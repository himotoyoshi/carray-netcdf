CArray/NetCDF library API document
==================================

1. Loading library
------------------

    require "netcdflib"
    include NC

2. Methods 
----------

### 2.0. Data types

    data_type = NC.ca_type(xtype)
    xtype     = NC.nc_type(data_type)

### 2.1. NetCDF File

    fd = nc_create(FILENAME[, mode=NC_CLOBBER])
       
       mode : NC_CLOBBER   - permit overwrite 
              NC_NOCLOBBER - inhibit overwrite
              NC_SHARE     - no buffering

    fd = nc_open(FILENAME[, mode=NC_NOWRITE])
       
       mode : NC_NOWRITE   - readonly
              NC_WRTIE     - writable
              NC_SHARE     - no buffering

    nc_close(fd)

    nc_redef(fd)
    nc_enddef(fd)

    nc_sync(fd)

    ndims    = nc_inq_ndims(fd)
    nvars    = nc_inq_nvars(fd)
    natts    = nc_inq_natts(fd)
    unlimdim = nc_inq_unlimdim(fd)

    oldmode  = nc_set_fill(mode)

       mode : NC_FILL
              NC_NOFILL

### 2.2. NetCDF Dimension

    dimid    = nc_def_dim(fd, dimname, len)
    dimid    = nc_inq_dimid(fd, dimname)    => Integer | nil

    dimname  = nc_inq_dimname(fd, dimid)
    dimlen   = nc_inq_dimlen(fd, dimid)

    nc_rename_dim(fd, dimid, newname)

### 2.3. NetCDF Variable

    varid    = nc_def_var(fd, varname, type, dim)
    varid    = nc_inq_varid(fd, varname)    => Integer | nil

    vartype  = nc_inq_vartype(fd, varid)
    varndims = nc_inq_varndims(fd, varid)
    vardimid = nc_inq_varndimid(fd, varid)  => Array of dimids

    nc_put_var1(fd, varid, [i,j,..], val)
    nc_put_var(fd, varid, ca)
    nc_put_vara(fd, varid, start, count, ca)
    nc_put_vars(fd, varid, start, count, stride, ca)
    nc_put_varm(fd, varid, start, count, stride, imap, ca)

      + with data_type conversion

    val = nc_get_var1(fd, varid, [i,j,..])
    ca = nc_get_var(fd, varid)                              [useful]
    ca = nc_get_vara(fd, varid, start, count)               [useful]
    ca = nc_get_vars(fd, varid, start, count, stride)       [useful]
    ca = nc_get_varm(fd, varid, start, count, stride, imap) [useful]

      + no data_type conversion

    nc_get_var(fd, varid, ca)                               [pedantic]
    nc_get_vara(fd, varid, start, count, ca)                [pedantic]
    nc_get_vars(fd, varid, start, count, stride, ca)        [pedantic]
    nc_get_varm(fd, varid, start, count, stride, imap, ca)  [pedantic]

      + with data_type conversion.

    nc_rename_var(fd, varid, newname)

### 2.4. NetCDF Attribute

    varnatts = nc_inq_varnatts(fd, varid)
    attid    = nc_inq_attid(fd, varid, attname)

    attname  = nc_inq_attname(fd, varid, attid)
    atttype  = nc_inq_atttype(fd, varid, attname)
    attlen   = nc_inq_attlen(fd, varid, attname)

    nc_put_att(fd, varid, attname, val)

      varid : NC_GLOBAL for global att
      val   : String  -> NC_CHAR
              Integer -> NC_INT
              Float   -> NC_DOUBLE
              CArray  -> NC.nc_type(val.data_type)


    val = nc_get_att(fd, varid, attname)  => String | Numeric | CArray | nil

      varid : NC_GLOBAL for global att

    nc_get_att(fd, varid, attname, ca)                 [pedantic]

    nc_rename_att(fd, varid, attname, newname)
    nc_del_att(fd, varid, attname)

    nc_copy_att(fd1, varid1, attname, fd2, varid2)

3. Constants
------------

    NC_NAT 
    NC_BYTE
    NC_SHORT
    NC_INT
    NC_FLOAT
    NC_DOUBLE

    NC_NOERR        - status of API routines

    NC_CLOBBER      - permit overwrite  [nc_create]
    NC_NOCLOBBER    - inhibit overwrite [nc_create]
    NC_NOWRITE      - readonly [nc_open]
    NC_WRITE        - writable [nc_open]
    NC_SHARE        - no buffering [nc_create, nc_open]

    NC_GLOBAL       - varid for global attributes

    NC_NOFILL       - nc_set_fill(fd, varid, NC_NOFILL)
    NC_FILL         - nc_set_fill(fd, varid, NC_FILL)

    NC_MAX_NAME
    NC_MAX_VAR_DIMS
    NC_MAX_DIMS

    NC_LOCK         - ???
    NC_SIZEHINT_DEFAULT - ???

