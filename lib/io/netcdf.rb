require "carray"
require "carray/netcdflib.so"

module NC

  module_function

  def nc_decode (fd, varid, data)
    if fill_value = nc_get_att(fd, varid, "_FillValue")
      data[:eq, fill_value] = UNDEF
    end
    if scale_factor = nc_get_att(fd, varid, "scale_factor")
      data *= scale_factor
    end
    if add_offset = nc_get_att(fd, varid, "add_offset")
      data += add_offset
    end
    return data
  end

  def nc_put_att_simple (fd, varid, name, val)
    case val
    when Float
      nc_put_att(fd, varid, name, CA_DOUBLE(val))
    when Integer
      nc_put_att(fd, varid, name, CA_INT(val))
    else
      nc_put_att(fd, varid, name, val)
    end
  end

  def nc_put_var_all (fd, varid, val)
    data_type = NC.ca_type(nc_inq_vartype(fd, varid))
    dim = (0...nc_inq_varndims(fd, varid)).collect do |dimid| 
      nc_inq_dimlen(fd, dimid)
    end
    data = CArray.new(data_type, dim) { val }
    nc_put_var(fd, varid, data)
  end

end

class NCObject

  include NC

  def get_attributes (file_id, var_id)
    attrs = {}
    varnatts = nc_inq_varnatts(file_id, var_id)
    varnatts.times do |i|
      attname = nc_inq_attname(file_id, var_id, i)
      value   = nc_get_att(file_id, var_id, attname) 
      attrs[attname] = value
    end
    return attrs.freeze
  end
  
  def attribute (name)
    return @attributes[name]
  end

  attr_reader :attributes

end

class NCVar < NCObject
  
  include NC
  
  def initialize (ncfile, var_id)
    @ncfile     = ncfile
    @file_id    = ncfile.file_id
    @var_id     = var_id
    @name       = nc_inq_varname(@file_id, var_id)
    @vartype    = nc_inq_vartype(@file_id, var_id)
    @dims       = ncfile.dims.values_at(*nc_inq_vardimid(@file_id, var_id))
    @shape      = @dims.map{|d| d.len}
    @attributes = get_attributes(@file_id, var_id)
    @dims.freeze
    @shape.freeze
  end
  
  attr_reader :name, :dims

  def definition
    {
      type: @vartype,
      dims: dims.map{|x| x.name },
      attributes: @attributes.dup
    }
  end

  def inspect
    return "#{@name}#{@dims}"
  end
  
  def is_dim? 
    begin
      nc_inq_dimlen(@file_id, @var_id)
      return true
    rescue RuntimeError
      return false
    end
  end

  def decode (value)
    if @attributes.has_key?("_FillValue")
      fill_value = @attributes["_FillValue"]
      case value
      when CArray
        value[:eq, fill_value] = UNDEF
      else
        value = UNDEF if value == fill_value
      end
    end
    if @attributes.has_key?("missing_value")
      missing_values = [@attributes["missing_value"]].flatten
      missing_values.each do |mv|
        case value
        when CArray
          value[:eq, mv] = UNDEF
        else
          if value == mv
            value = UNDEF 
            break
          end
        end
      end
    end
    if @attributes.has_key?("scale_factor")
      value *= @attributes["scale_factor"]
    end
    if @attributes.has_key?("add_offset")
      value += @attributes["add_offset"]
    end
    return value    
  end

  def to_ca
    return self[]
  end

  def [] (*argv)
    return get!(*argv)
  end

  def get (*argv)
    if argv.size > 0 and argv[0].is_a?(Struct::CAIndexInfo)
      info = argv.shift
    else
      info = CArray.scan_index(@shape, argv)
    end
    out  = nil
    case info.type
    when CA_REG_ADDRESS
      addr  = info.index[0]
      index = []
      (0..@shape.size-1).reverse_each do |i|
        index[i] = addr % @shape[i]
        addr /= @shape[i]
      end
      out = get_var1(*index)
    when CA_REG_FLATTEN
      out = get_var[nil]
    when CA_REG_POINT
      out = get_var1(*info.index)
    when CA_REG_ALL
      out = get_var()
    when CA_REG_BLOCK
      start = []
      count = []
      stride = []
      info.index.each do |idx|
        case idx
        when Array
          start << idx[0]
          count << idx[1]
          stride << idx[2]
        else
          start << idx
          count << 1
          stride << 1
        end
      end
      if stride.all?{|x| x == 1 }
        out = get_vara(start, count)
      else
        out = get_vars(start, count, stride)
      end
    when CA_REG_SELECT, CA_REG_GRID
      out = get_var[*argv]
    else
      raise "invalid index"
    end
    case out
    when CArray
      return out.compact
    else
      return out
    end
  end
  
  def get! (*argv)
    info = CArray.scan_index(@shape, argv)
    case info.type
    when CA_REG_METHOD_CALL
      return decode(get_var)[*argv]
    else
      return decode(get(info, *argv))
    end
  end
   
  def get_var1 (*index)
    return nc_get_var1(@file_id, @var_id, index)
  end

  def get_var1! (*index)
    return decode(get_var1(*index))
  end

  def get_var ()
    return nc_get_var(@file_id, @var_id)
  end

  def get_var! ()
    return decode(nc_get_var(@file_id, @var_id))
  end

  def get_vara (start, count)
    return nc_get_vara(@file_id, @var_id, start, count)
  end

  def get_vara! (start, count)
    return decode(nc_get_vara(@file_id, @var_id, start, count))
  end

  def get_vars (start, count, stride)
    return nc_get_vars(@file_id, @var_id, start, count, stride)
  end

  def get_vars! (start, count, stride)
    return decode(nc_get_vars(@file_id, @var_id, start, count, stride))
  end

  def get_varm (start, count, stride, imap)
    return nc_get_varm(@file_id, @var_id, start, count, stride, imap)
  end

  def get_varm! (start, count, stride, imap)
    return decode(nc_get_varm(@file_id, @var_id, start, count, stride, imap))
  end

end

class NCDim < NCObject
  
  include NC
  
  def initialize (ncfile, dim_id)
    @ncfile      = ncfile
    @file_id     = ncfile.file_id
    @dim_id      = dim_id
    @name        = nc_inq_dimname(@file_id, @dim_id)
    @len         = nc_inq_dimlen(@file_id, @dim_id)
  end

  attr_reader :name, :len

  def definition
    return @len
  end

  def inspect
    return "#{@name}=#{@len}"
  end
  
  def to_i
    return @len
  end

  def to_ca
    return self[]
  end

  def [] (*argv)
    return @ncfile[name][*argv]
  end

end

class NCFile < NCObject

  include NC

  def self.open (filename)
    file_id = NC.open(filename)
    return NCFile.new(file_id)
  end

  def initialize (file_id)
    @file_id  = file_id
    @dims     = []
    @vars     = []
    @name2dim = {}
    @name2var = {}
    @attributes = get_attributes(@file_id, NC::NC_GLOBAL)
    parse_metadata()
  end

  attr_reader :file_id, :dims, :vars

  def parse_metadata ()
    ndims = nc_inq_ndims(@file_id)
    ndims.times do |i|
      dim = NCDim.new(self, i)
      @dims[i] = dim
      @name2dim[dim.name] = dim
    end
    @dims.freeze
    @name2dim.freeze
    nvars = nc_inq_nvars(@file_id)
    nvars.times do |i|
      var = NCVar.new(self, i)
      @vars[i] = var
      @name2var[var.name] = var
    end
    @vars.freeze
    @name2var.freeze
  end

  def definition
    {
      dims: @dims.map{|x| [x.name, x.definition] }.to_h,
      vars: @vars.map{|x| [x.name, x.definition] }.to_h,
      attributes: @attributes.dup
    }
  end

  def [] (name)
    return @name2var[name]
  end
  
  def dim (name)
    return @name2dim[name]
  end

  def has_dim?(name)
    return @name2dim.has_key?(name)
  end

  def has_var?(name)
    return @name2var.has_key?(name)
  end

end

class NCFileWriter
  
  include NC
  
  class Var

    include NC

    def initialize (ncfile, name, definition)
      @ncfile  = ncfile
      @file_id = ncfile.file_id
      @definition = definition
      @name    = name
      @type    = definition[:type]
      @dims    = definition[:dims]
      @dim_ids = @dims.map{|key| @ncfile.dim(key).dim_id }
      @shape   = @dims.map{|key| @ncfile.dim(key).to_i }
      @var_id  = nc_def_var(@file_id, @name, @type, @dim_ids)
      @attributes = definition[:attributes].map{|key, value| [key.to_s, value]}.to_h.freeze
      @attributes.each do |name, value|
        nc_put_att(@file_id, @var_id, name, value)
      end
    end
    
    attr_reader :name, :attributes
    
    def []= (*argv)
      put(*argv)      
    end

    def put (*argv)
      value = argv.pop
      info = CArray.scan_index(@shape, argv)
      case info.type
      when CA_REG_ADDRESS
        addr  = info.index[0]
        index = []
        (0..@shape.size-1).reverse_each do |i|
          index[i] = addr % @shape[i]
          addr /= @shape[i]
        end
        put_var1(index, value)
      when CA_REG_FLATTEN
        put_var(value)
      when CA_REG_POINT
        put_var1(info.index, value)
      when CA_REG_ALL
        put_var(value)
      when CA_REG_BLOCK
        start = []
        count = []
        stride = []
        info.index.each do |idx|
          case idx
          when Array
            start << idx[0]
            count << idx[1]
            stride << idx[2]
          else
            start << idx
            count << 1
            stride << 1
          end
        end
        if stride.all?{|x| x == 1 }
          put_vara(start, count, value)
        else
          put_vars(start, count, stride, value)
        end
      else
        raise "invalid index"
      end
    end

    def put_var1 (index, value)
      return nc_put_var1(@file_id, @var_id, index, value)
    end

    def put_var (value)
      return nc_put_var(@file_id, @var_id, value)
    end

    def put_vara (start, count, value)
      return nc_put_vara(@file_id, @var_id, start, count, value)
    end

    def put_vars (start, count, stride, value)
      return nc_put_vars(@file_id, @var_id, start, count, stride, value)
    end

    def get_varm (start, count, stride, imap, value)
      return nc_put_varm(@file_id, @var_id, start, count, stride, imap, value)
    end

  
  end
  
  class Dim

    include NC
    
    def initialize (ncfile, name, len)
      @ncfile     = ncfile
      @file_id    = ncfile.file_id
      @name       = name
      @len        = len
      @dim_id     = nc_def_dim(@file_id, @name, @len)
    end
    
    attr_reader :name, :dim_id
    
    def to_i
      return @len
    end
    
  end
  
  def initialize (file)
    @file_id = nc_create(file)
    @dims    = []
    @name2dim = {}
    @vars    = []
    @name2var = {}
    @attributes = nil
  end
  
  attr_reader :file_id
  
  def define (definition)
    definition[:dims].each do |name, len|
      dim = Dim.new(self, name.to_s, len.to_i)
      @dims.push dim
      @name2dim[name.to_s] = dim
    end
    definition[:vars].each do |name, info|
      var = Var.new(self, name.to_s, info)
      @vars.push var
      @name2var[name.to_s] = var
    end
    @attributes = definition[:attributes].map{|key, value| [key.to_s, value]}.to_h.freeze
    @attributes.each do |name, value|
      nc_put_att(@file_id, NC_GLOBAL, name, value)
    end
    nc_enddef(@file_id)    
  end
  
  def copy_dims (nc, name = nil)
    if name
      if @name2dim.has_key?(name) and @name2var.has_key?(name)
        @name2var[name].put(nc[name].get)
      end
    else
      nc.dims.each do |dim|
        if @name2dim.has_key?(dim.name) and @name2var.has_key?(dim.name)
          @name2var[dim.name].put(nc[dim.name].get)
        end
      end
    end
  end
  
  def dim (name)
    return @name2dim[name]
  end

  def [] (name)
    return @name2var[name]
  end

  def []= (name, value)
    return @name2var[name].put(value)
  end
  
  def close
    nc_close(@file_id)
  end
  
end