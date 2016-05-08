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
