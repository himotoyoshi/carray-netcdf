#include "ruby.h"
#include "carray.h"
#include <netcdf.h>

#define CHECK_ARGC(n) \
  if ( argc != n ) \
    rb_raise(rb_eRuntimeError, "invalid # of argumnet (%i for %i)", argc, n)

#define CHECK_TYPE_STRING(val) \
  if ( TYPE(val) != T_STRING ) \
    rb_raise(rb_eRuntimeError, "string type arg required")

#define CHECK_TYPE_STRING_OR_NIL(val) \
  if ( TYPE(val) != T_STRING && !NIL_P(val)) \
    rb_raise(rb_eRuntimeError, "string type arg required")

#define CHECK_TYPE_ARRAY(val) \
  if ( TYPE(val) != T_ARRAY ) \
    rb_raise(rb_eRuntimeError, "array type arg required")

#define CHECK_TYPE_ARRAY_OR_NIL(val) \
  if ( TYPE(val) != T_ARRAY && ! NIL_P(val) )   \
    rb_raise(rb_eRuntimeError, "array type arg or nil required")

#define CHECK_TYPE_INT(val) \
  if ( ! rb_obj_is_kind_of(val, rb_cInteger) ) \
    rb_raise(rb_eRuntimeError, "int type arg required")

#define CHECK_TYPE_NUMERIC(val) \
  if ( ! rb_obj_is_kind_of(val, rb_cNumeric) ) \
    rb_raise(rb_eRuntimeError, "int type arg required")

#define CHECK_TYPE_ID(val) \
  if ( ! rb_obj_is_kind_of(val, rb_cInteger) ) \
    rb_raise(rb_eRuntimeError, "id must be an integer")

#define CHECK_TYPE_DATA(val) \
  if ( ! rb_obj_is_kind_of(val, rb_cCArray) ) \
    rb_raise(rb_eRuntimeError, "CArray type arg required")

#define CHECK_STATUS(status) \
    if ( (status) != NC_NOERR )	\
      rb_raise(rb_eRuntimeError, "%s", nc_strerror(status))

static VALUE mNetCDF;

static int
rb_nc_typemap (nc_type nc_type)
{
  switch ( nc_type ) {
  case NC_CHAR:
    return CA_INT8;
  case NC_BYTE: 
    return CA_UINT8;
  case NC_SHORT:
    return CA_INT16;
  case NC_INT:
    return CA_INT32;
  case NC_FLOAT:
    return CA_FLOAT32;
  case NC_DOUBLE:
    return CA_FLOAT64;
  default:
    rb_raise(rb_eRuntimeError, "invalid NC_TYPE");
  }
}

static nc_type
rb_nc_rtypemap (int ca_type)
{
  switch ( ca_type ) {
  case CA_INT8:
    return NC_BYTE;
  case CA_UINT8:
    return NC_BYTE;
  case CA_INT16:
    return NC_SHORT;
  case CA_INT32:
    return NC_INT;
  case CA_FLOAT32:
    return NC_FLOAT;
  case CA_FLOAT64:
    return NC_DOUBLE;
  default:
    rb_raise(rb_eRuntimeError, "invalid CA_TYPE");
  }
}

static VALUE
rb_nc_ca_type (int argc, VALUE *argv, VALUE mod)
{
  int type;

  CHECK_ARGC(1);
  CHECK_TYPE_INT(argv[0]);
  
  type = rb_nc_typemap(NUM2LONG(argv[0]));

  return LONG2NUM(type);
}

static VALUE
rb_nc_nc_type (int argc, VALUE *argv, VALUE mod)
{
  nc_type type;

  CHECK_ARGC(1);
  CHECK_TYPE_INT(argv[0]);
  
  type = rb_nc_rtypemap(NUM2LONG(argv[0]));

  return LONG2NUM(type);
}

static VALUE
rb_nc_create (int argc, VALUE *argv, VALUE mod)
{
  int status, nc_id;

  if ( argc < 1 ) {
    rb_raise(rb_eArgError, "invalid # of arguments");
  }

  CHECK_TYPE_STRING(argv[0]);

  if ( argc == 1 ) {
    status = nc_create(StringValuePtr(argv[0]), NC_CLOBBER, &nc_id);
  }
  else {
    CHECK_TYPE_INT(argv[1]);
    status = nc_create(StringValuePtr(argv[0]), NUM2LONG(argv[1]), &nc_id);
  }

  CHECK_STATUS(status);

  return LONG2NUM(nc_id);
}

static VALUE
rb_nc_open (int argc, VALUE *argv, VALUE mod)
{
  int status, nc_id;

  if ( argc < 1 ) {
    rb_raise(rb_eArgError, "invalid # of arguments");
  }

  CHECK_TYPE_STRING(argv[0]);

  if ( argc == 1 ) {
    status = nc_open(StringValuePtr(argv[0]), NC_NOWRITE, &nc_id);
  }
  else {
    CHECK_TYPE_INT(argv[1]);
    status = nc_open(StringValuePtr(argv[0]), NUM2LONG(argv[1]), &nc_id);
  }

  CHECK_STATUS(status);

  return LONG2NUM(nc_id);
}

static VALUE
rb_nc_close (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(1);
  CHECK_TYPE_ID(argv[0]);
  
  status = nc_close(NUM2LONG(argv[0]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

static VALUE
rb_nc_redef (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(1);
  CHECK_TYPE_ID(argv[0]);
  
  status = nc_redef(NUM2LONG(argv[0]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

static VALUE
rb_nc_enddef (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(1);
  CHECK_TYPE_ID(argv[0]);
  
  status = nc_enddef(NUM2LONG(argv[0]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

static VALUE
rb_nc_sync (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(1);
  CHECK_TYPE_ID(argv[0]);
  
  status = nc_sync(NUM2LONG(argv[0]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

static VALUE
rb_nc_inq_ndims (int argc, VALUE *argv, VALUE mod)
{
  int status, ndims;

  CHECK_ARGC(1);
  CHECK_TYPE_ID(argv[0]);
  
  status = nc_inq_ndims(NUM2LONG(argv[0]), &ndims);

  CHECK_STATUS(status);

  return LONG2NUM(ndims);
}

static VALUE
rb_nc_inq_nvars (int argc, VALUE *argv, VALUE mod)
{
  int status, nvars;

  CHECK_ARGC(1);
  CHECK_TYPE_ID(argv[0]);
  
  status = nc_inq_nvars(NUM2LONG(argv[0]), &nvars);

  CHECK_STATUS(status);

  return LONG2NUM(nvars);
}

static VALUE
rb_nc_inq_natts (int argc, VALUE *argv, VALUE mod)
{
  int status, natts;

  CHECK_ARGC(1);
  CHECK_TYPE_ID(argv[0]);
  
  status = nc_inq_natts(NUM2LONG(argv[0]), &natts);

  CHECK_STATUS(status);

  return LONG2NUM(natts);
}

static VALUE
rb_nc_inq_unlimdim (int argc, VALUE *argv, VALUE mod)
{
  int status, uldim;

  CHECK_ARGC(1);
  CHECK_TYPE_ID(argv[0]);
  
  status = nc_inq_unlimdim(NUM2LONG(argv[0]), &uldim);

  CHECK_STATUS(status);

  return LONG2NUM(uldim);
}

static VALUE
rb_nc_inq_dimid (int argc, VALUE *argv, VALUE mod)
{
  int status, dimid;

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_STRING(argv[1]);
  
  status = nc_inq_dimid(NUM2LONG(argv[0]), StringValuePtr(argv[1]), &dimid);

  return ( status != NC_NOERR ) ? Qnil : LONG2NUM(dimid);
}

static VALUE
rb_nc_inq_varid (int argc, VALUE *argv, VALUE mod)
{
  int status, varid;

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_STRING(argv[1]);
  
  status = nc_inq_varid(NUM2LONG(argv[0]), StringValuePtr(argv[1]), &varid);

  return ( status != NC_NOERR ) ? Qnil : LONG2NUM(varid);
}

static VALUE
rb_nc_inq_attid (int argc, VALUE *argv, VALUE mod)
{
  int status, attid;

  CHECK_ARGC(3);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);
  
  status = nc_inq_attid(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
		                    StringValuePtr(argv[2]), &attid);

  return ( status != NC_NOERR ) ? Qnil : LONG2NUM(attid);
}

static VALUE
rb_nc_inq_dimlen (int argc, VALUE *argv, VALUE mod)
{
  size_t dimlen;
  int status;

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  
  status = nc_inq_dimlen(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &dimlen);

  CHECK_STATUS(status);

  return ULONG2NUM(dimlen);
}

static VALUE
rb_nc_inq_dimname (int argc, VALUE *argv, VALUE mod)
{
  int status;
  char dimname[NC_MAX_NAME];

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  
  status = nc_inq_dimname(NUM2LONG(argv[0]), NUM2LONG(argv[1]), dimname);

  CHECK_STATUS(status);

  return rb_str_new2(dimname);
}

static VALUE
rb_nc_inq_varname (int argc, VALUE *argv, VALUE mod)
{
  int status;
  char varname[NC_MAX_NAME];

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  
  status = nc_inq_varname(NUM2LONG(argv[0]), NUM2LONG(argv[1]), varname);

  CHECK_STATUS(status);

  return rb_str_new2(varname);
}

static VALUE
rb_nc_inq_vartype (int argc, VALUE *argv, VALUE mod)
{
  int status;
  nc_type type;

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  
  status = nc_inq_vartype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &type);

  CHECK_STATUS(status);

  return LONG2NUM(type);
}

static VALUE
rb_nc_inq_varndims (int argc, VALUE *argv, VALUE mod)
{
  int status, ndims;

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  
  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  return LONG2NUM(ndims);
}

static VALUE
rb_nc_inq_vardimid (int argc, VALUE *argv, VALUE mod)
{
  volatile VALUE rdim;
  int dimid[NC_MAX_DIMS];
  int ndims;
  int status;
  int i;

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]); /* nc_id */
  CHECK_TYPE_ID(argv[1]); /* var_id */

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  status = nc_inq_vardimid(NUM2LONG(argv[0]), NUM2LONG(argv[1]), dimid);

  CHECK_STATUS(status);

  rdim = rb_ary_new();
  for (i=0; i<ndims; i++) {
    rb_ary_store(rdim, i, ULONG2NUM(dimid[i]));
  }

  return rdim;
}

static VALUE
rb_nc_inq_varnatts (int argc, VALUE *argv, VALUE mod)
{
  int status, natts;

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  
  status = nc_inq_varnatts(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &natts);

  CHECK_STATUS(status);

  return LONG2NUM(natts);
}


static VALUE
rb_nc_inq_attname (int argc, VALUE *argv, VALUE mod)
{
  int status;
  char attname[NC_MAX_NAME];

  CHECK_ARGC(3);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_ID(argv[2]);
  
  status = nc_inq_attname(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			  NUM2LONG(argv[2]), attname);

  CHECK_STATUS(status);

  return rb_str_new2(attname);
}

static VALUE
rb_nc_inq_atttype (int argc, VALUE *argv, VALUE mod)
{
  int status;
  nc_type type;

  CHECK_ARGC(3);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);
  
  status = nc_inq_atttype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			  StringValuePtr(argv[2]), &type);

  CHECK_STATUS(status);

  return LONG2NUM(type);
}

static VALUE
rb_nc_inq_attlen (int argc, VALUE *argv, VALUE mod)
{
  size_t len;
  int status;

  CHECK_ARGC(3);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);
  
  status = nc_inq_attlen(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			 StringValuePtr(argv[2]), &len);

  CHECK_STATUS(status);

  return LONG2NUM(len);
}

static VALUE
rb_nc_def_dim (int argc, VALUE *argv, VALUE mod)
{
  int status, dimid;

  CHECK_ARGC(3);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_STRING(argv[1]);
  CHECK_TYPE_INT(argv[2]);
  
  status = nc_def_dim(NUM2LONG(argv[0]), StringValuePtr(argv[1]), 
		      NUM2LONG(argv[2]), &dimid);

  CHECK_STATUS(status);

  return LONG2NUM(dimid);
}

static VALUE
rb_nc_def_var (int argc, VALUE *argv, VALUE mod)
{
  volatile VALUE vdim;
  int status, varid;
  int ndims;
  int dimids[NC_MAX_DIMS];
  int i;

  CHECK_ARGC(4);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_STRING(argv[1]);
  CHECK_TYPE_INT(argv[2]);
  CHECK_TYPE_ARRAY(argv[3]);

  vdim = argv[3];

  ndims = RARRAY_LEN(vdim);
  for (i=0; i<ndims; i++) {
    dimids[i] = NUM2LONG(RARRAY_PTR(vdim)[i]);
  }

  status = nc_def_var(NUM2LONG(argv[0]), StringValuePtr(argv[1]), 
		      NUM2LONG(argv[2]), ndims, dimids, &varid);

  CHECK_STATUS(status);

  return LONG2NUM(varid);
}


static VALUE
rb_nc_del_att (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(3);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);

  status = nc_del_att(NUM2LONG(argv[0]), NUM2LONG(argv[1]), StringValuePtr(argv[2]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

static int
nc_get_att_numeric (int ncid, int varid, const char name[], 
		    nc_type type, void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_get_att_uchar(ncid, varid, name, value);
  case NC_CHAR:
    return nc_get_att_schar(ncid, varid, name, value);
  case NC_SHORT:
    return nc_get_att_short(ncid, varid, name, value);
  case NC_INT:
    return nc_get_att_int(ncid, varid, name, value);
  case NC_FLOAT:
    return nc_get_att_float(ncid, varid, name, value);
  case NC_DOUBLE:
    return nc_get_att_double(ncid, varid, name, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_put_att_numeric (int ncid, int varid, const char name[], 
		    nc_type type, nc_type xtype, size_t len, void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_put_att_uchar(ncid, varid, name, xtype, len, value);
  case NC_CHAR:
    return nc_put_att_schar(ncid, varid, name, xtype, len, value);
  case NC_SHORT:
    return nc_put_att_short(ncid, varid, name, xtype, len, value);
  case NC_INT:
    return nc_put_att_int(ncid, varid, name, xtype, len, value);
  case NC_FLOAT:
    return nc_put_att_float(ncid, varid, name, xtype, len, value);
  case NC_DOUBLE:
    return nc_put_att_double(ncid, varid, name, xtype, len, value);
  default:
    return NC_EBADTYPE;
  }
}

static VALUE
rb_nc_get_att (int argc, VALUE *argv, VALUE mod)
{
  nc_type type;
  size_t len;
  int status;

  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);

  status = nc_inq_atttype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			  StringValuePtr(argv[2]), &type);

  if ( status != NC_NOERR) {
    return Qnil;
  }

  status = nc_inq_attlen(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			 StringValuePtr(argv[2]), &len);
  CHECK_STATUS(status);

  if ( argc == 3 ) {

    if ( type == NC_CHAR ) {
      volatile VALUE text = rb_str_new(NULL, len);
      status = nc_get_att_text(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       StringValuePtr(argv[2]), 
			       StringValuePtr(text));
      return text;
    }
    else if ( len == 1 ) {
      switch ( type ) {
      case NC_BYTE: {
	uint8_t val;
	status = nc_get_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				  StringValuePtr(argv[2]), type, &val);
	CHECK_STATUS(status);
	return INT2NUM(val);
      }
      case NC_SHORT: {
	int16_t val;
	status = nc_get_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				    StringValuePtr(argv[2]), type, &val);
	CHECK_STATUS(status);
	return INT2NUM(val);
      }
      case NC_INT: {
	int32_t val;
	status = nc_get_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				    StringValuePtr(argv[2]), type, &val);
	CHECK_STATUS(status);
	return INT2NUM(val);
      }
      case NC_FLOAT: {
	float32_t val;
	status = nc_get_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				    StringValuePtr(argv[2]), type, &val);
	CHECK_STATUS(status);
	return rb_float_new(val);
      }
      case NC_DOUBLE: {
	float64_t val;
	status = nc_get_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				    StringValuePtr(argv[2]), type, &val);
	CHECK_STATUS(status);
	return rb_float_new(val);
      }
      default: 
	rb_raise(rb_eRuntimeError, "unknown att nc_type");
      }
    }
    else {
      volatile VALUE out;
      CArray *ca;
      int8_t  data_type;
      ca_size_t dim0 = len;

      data_type = rb_nc_typemap (type);
      out = rb_carray_new(data_type, 1, &dim0, 0, NULL);

      Data_Get_Struct(out, CArray, ca);

      status = nc_get_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				  StringValuePtr(argv[2]), type, ca->ptr);

      CHECK_STATUS(status);

      return out;
    }
  } 
  else {

    CHECK_ARGC(4);

    if ( type == NC_CHAR ) {
      volatile VALUE text = argv[3];
      rb_str_resize(text, len);
      status = nc_get_att_text(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       StringValuePtr(argv[2]), 
			       StringValuePtr(text));
    }  
    else {
      CArray *ca;
      nc_type xtype;
      if ( ! rb_obj_is_kind_of(argv[3], rb_cCArray) ) {
	rb_raise(rb_eTypeError, "arg4 must be a CArray object");
      }
      Data_Get_Struct(argv[3], CArray, ca);
      xtype = rb_nc_rtypemap(ca->data_type);
      ca_attach(ca);
      status = nc_get_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				  StringValuePtr(argv[2]), type, ca->ptr);
      ca_sync(ca);
      ca_detach(ca);
    }
    
    CHECK_STATUS(status);

    return LONG2NUM(status);
  }
}

static VALUE
rb_nc_put_att (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(4);

  if ( TYPE(argv[3]) == T_STRING ) {
    volatile VALUE text = argv[3];
    status = nc_put_att_text(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			     StringValuePtr(argv[2]), 
			     strlen(StringValuePtr(text)), StringValuePtr(text));
  }
  else if ( rb_obj_is_kind_of(argv[3], rb_cInteger) ) {
    int32_t val = NUM2INT(argv[3]);
    status = nc_put_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				StringValuePtr(argv[2]), 
				NC_INT, NC_INT, 1, &val);
  }
  else if ( rb_obj_is_kind_of(argv[3], rb_cFloat) ) {
    float64_t val = NUM2DBL(argv[3]);
    status = nc_put_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				StringValuePtr(argv[2]), 
				NC_DOUBLE, NC_DOUBLE, 1, &val);
  }
  else {
    CArray *ca;
    nc_type xtype;
    if ( ! rb_obj_is_kind_of(argv[3], rb_cCArray) ) {
      rb_raise(rb_eTypeError, "arg4 must be a CArray object");
    }
    Data_Get_Struct(argv[3], CArray, ca);
    xtype = rb_nc_rtypemap(ca->data_type);
    ca_attach(ca);
    status = nc_put_att_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				StringValuePtr(argv[2]), 
				xtype, xtype, ca->elements, ca->ptr);
    ca_detach(ca);
  }

  CHECK_STATUS(status);

  return LONG2NUM(status);
}


static int
nc_get_var1_numeric (int ncid, int varid, 
		     nc_type type, size_t index[], void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_get_var1_uchar(ncid, varid, index, value);
  case NC_CHAR:
    return nc_get_var1_schar(ncid, varid, index, value);
  case NC_SHORT:
    return nc_get_var1_short(ncid, varid, index, value);
  case NC_INT:
    return nc_get_var1_int(ncid, varid, index, value);
  case NC_FLOAT:
    return nc_get_var1_float(ncid, varid, index, value);
  case NC_DOUBLE:
    return nc_get_var1_double(ncid, varid, index, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_put_var1_numeric (int ncid, int varid, 
		     nc_type type, size_t index[], void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_put_var1_uchar(ncid, varid, index, value);
  case NC_CHAR:
    return nc_put_var1_schar(ncid, varid, index, value);
  case NC_SHORT:
    return nc_put_var1_short(ncid, varid, index, value);
  case NC_INT:
    return nc_put_var1_int(ncid, varid, index, value);
  case NC_FLOAT:
    return nc_put_var1_float(ncid, varid, index, value);
  case NC_DOUBLE:
    return nc_put_var1_double(ncid, varid, index, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_get_var_numeric (int ncid, int varid, nc_type type, void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_get_var_uchar(ncid, varid, value);
  case NC_CHAR:
    return nc_get_var_schar(ncid, varid, value);
  case NC_SHORT:
    return nc_get_var_short(ncid, varid, value);
  case NC_INT:
    return nc_get_var_int(ncid, varid, value);
  case NC_FLOAT:
    return nc_get_var_float(ncid, varid, value);
  case NC_DOUBLE:
    return nc_get_var_double(ncid, varid, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_put_var_numeric (int ncid, int varid, nc_type type, void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_put_var_uchar(ncid, varid, value);
  case NC_CHAR:
    return nc_put_var_schar(ncid, varid, value);
  case NC_SHORT:
    return nc_put_var_short(ncid, varid, value);
  case NC_INT:
    return nc_put_var_int(ncid, varid, value);
  case NC_FLOAT:
    return nc_put_var_float(ncid, varid, value);
  case NC_DOUBLE:
    return nc_put_var_double(ncid, varid, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_get_vara_numeric (int ncid, int varid, nc_type type, 
		     const size_t start[], const size_t count[], 
		     void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_get_vara_uchar(ncid, varid, start, count, value);
  case NC_CHAR:
    return nc_get_vara_schar(ncid, varid, start, count, value);
  case NC_SHORT:
    return nc_get_vara_short(ncid, varid, start, count, value);
  case NC_INT:
    return nc_get_vara_int(ncid, varid, start, count, value);
  case NC_FLOAT:
    return nc_get_vara_float(ncid, varid, start, count, value);
  case NC_DOUBLE:
    return nc_get_vara_double(ncid, varid, start, count, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_put_vara_numeric (int ncid, int varid, nc_type type, 
		     const size_t start[], const size_t count[], 
		     void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_put_vara_uchar(ncid, varid, start, count, value);
  case NC_CHAR:
    return nc_put_vara_schar(ncid, varid, start, count, value);
  case NC_SHORT:
    return nc_put_vara_short(ncid, varid, start, count, value);
  case NC_INT:
    return nc_put_vara_int(ncid, varid, start, count, value);
  case NC_FLOAT:
    return nc_put_vara_float(ncid, varid, start, count, value);
  case NC_DOUBLE:
    return nc_put_vara_double(ncid, varid, start, count, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_get_vars_numeric (int ncid, int varid, nc_type type, 
		     const size_t start[], const size_t count[], 
		     const ptrdiff_t stride[],
		     void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_get_vars_uchar(ncid, varid, start, count, stride, value);
  case NC_CHAR:
    return nc_get_vars_schar(ncid, varid, start, count, stride, value);
  case NC_SHORT:
    return nc_get_vars_short(ncid, varid, start, count, stride, value);
  case NC_INT:
    return nc_get_vars_int(ncid, varid, start, count, stride, value);
  case NC_FLOAT:
    return nc_get_vars_float(ncid, varid, start, count, stride, value);
  case NC_DOUBLE:
    return nc_get_vars_double(ncid, varid, start, count, stride, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_put_vars_numeric (int ncid, int varid, nc_type type, 
		     const size_t start[], const size_t count[], 
		     const ptrdiff_t stride[],
		     void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_put_vars_uchar(ncid, varid, start, count, stride, value);
  case NC_CHAR:
    return nc_put_vars_schar(ncid, varid, start, count, stride, value);
  case NC_SHORT:
    return nc_put_vars_short(ncid, varid, start, count, stride, value);
  case NC_INT:
    return nc_put_vars_int(ncid, varid, start, count, stride, value);
  case NC_FLOAT:
    return nc_put_vars_float(ncid, varid, start, count, stride, value);
  case NC_DOUBLE:
    return nc_put_vars_double(ncid, varid, start, count, stride, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_get_varm_numeric (int ncid, int varid, nc_type type, 
		     const size_t start[], const size_t count[], 
		     const ptrdiff_t stride[], const ptrdiff_t imap[],
		     void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_get_varm_uchar(ncid, varid, start, count, stride, imap, value);
  case NC_CHAR:
    return nc_get_varm_schar(ncid, varid, start, count, stride, imap, value);
  case NC_SHORT:
    return nc_get_varm_short(ncid, varid, start, count, stride, imap, value);
  case NC_INT:
    return nc_get_varm_int(ncid, varid, start, count, stride, imap, value);
  case NC_FLOAT:
    return nc_get_varm_float(ncid, varid, start, count, stride, imap, value);
  case NC_DOUBLE:
    return nc_get_varm_double(ncid, varid, start, count, stride, imap, value);
  default:
    return NC_EBADTYPE;
  }
}

static int
nc_put_varm_numeric (int ncid, int varid, nc_type type, 
		     const size_t start[], const size_t count[], 
		     const ptrdiff_t stride[], const ptrdiff_t imap[],
		     void *value)
{
  switch (type) {
  case NC_BYTE:
    return nc_put_varm_uchar(ncid, varid, start, count, stride, imap, value);
  case NC_CHAR:
    return nc_put_varm_schar(ncid, varid, start, count, stride, imap, value);
  case NC_SHORT:
    return nc_put_varm_short(ncid, varid, start, count, stride, imap, value);
  case NC_INT:
    return nc_put_varm_int(ncid, varid, start, count, stride, imap, value);
  case NC_FLOAT:
    return nc_put_varm_float(ncid, varid, start, count, stride, imap, value);
  case NC_DOUBLE:
    return nc_put_varm_double(ncid, varid, start, count, stride, imap, value);
  default:
    return NC_EBADTYPE;
  }
}

static VALUE
rb_nc_get_var1 (int argc, VALUE *argv, VALUE mod)
{
  int status;
  nc_type type;
  int ndims;
  size_t index[NC_MAX_DIMS];
  int i;

  if ( argc < 3 ) {
    rb_raise(rb_eArgError, "invalid # of arguments");
  }

  status = nc_inq_vartype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &type);

  CHECK_STATUS(status);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  Check_Type(argv[2], T_ARRAY);
  for (i=0; i<ndims; i++) {
    index[i] = NUM2ULONG(RARRAY_PTR(argv[2])[i]);
  }

  if ( argc == 3 ) {
    switch ( type ) {
    case NC_BYTE: {
      uint8_t val;
      status = nc_get_var1_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				   type, index, &val);
      CHECK_STATUS(status);
      return INT2NUM(val);
    }
    case NC_SHORT: {
      int16_t val;
      status = nc_get_var1_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				   type, index, &val);
      CHECK_STATUS(status);
      return INT2NUM(val);
    }
    case NC_INT: {
      int32_t val;
      status = nc_get_var1_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				   type, index, &val);
      CHECK_STATUS(status);
      return INT2NUM(val);
    }
    case NC_FLOAT: {
      float32_t val;
      status = nc_get_var1_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				   type, index, &val);
      CHECK_STATUS(status);
      return rb_float_new(val);
    }
    case NC_DOUBLE: {
      float64_t val;
      status = nc_get_var1_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				   type, index, &val);
      CHECK_STATUS(status);
      return rb_float_new(val);
    }
    default: 
      rb_raise(rb_eRuntimeError, "unknown att nc_type");
    }
  }
  else {
    volatile VALUE data = argv[3];
    CArray *ca;

    if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
      rb_raise(rb_eTypeError, "arg4 must be a CArray object");
    }

    Data_Get_Struct(data, CArray, ca);

    type = rb_nc_rtypemap(ca->data_type);

    ca_attach(ca);
    status = nc_get_var1_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, index, ca->ptr);

    ca_sync(ca);
    ca_detach(ca);

    CHECK_STATUS(status);

    return LONG2NUM(status);
  }

}

static VALUE
rb_nc_put_var1 (int argc, VALUE *argv, VALUE mod)
{
  volatile VALUE data;
  int status;
  nc_type type;
  size_t index[NC_MAX_DIMS];
  int ndims;
  CArray *ca;
  int i;

  CHECK_ARGC(4);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  Check_Type(argv[2], T_ARRAY);

  for (i=0; i<ndims; i++) {
    index[i] = NUM2ULONG(RARRAY_PTR(argv[2])[i]);
  }

  data = argv[3];

  if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
    rb_raise(rb_eTypeError, "arg4 must be a CArray object");
  }

  Data_Get_Struct(data, CArray, ca);

  type = rb_nc_rtypemap(ca->data_type);

  ca_attach(ca);
  status = nc_put_var1_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, index, ca->ptr);
  ca_detach(ca);

  CHECK_STATUS(status);
  
  return LONG2NUM(status);
}

static VALUE
rb_nc_get_var (int argc, VALUE *argv, VALUE mod)
{
  int status;
  int ndims;
  int dimid[NC_MAX_DIMS];
  nc_type type;

  if ( argc < 2 ) {
    rb_raise(rb_eArgError, "invalid # of arguments");
  }
  
  status = nc_inq_vartype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &type);

  CHECK_STATUS(status);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  status = nc_inq_vardimid(NUM2LONG(argv[0]), NUM2LONG(argv[1]), dimid);

  CHECK_STATUS(status);

  if ( argc == 2 ) {
    volatile VALUE out;
    CArray *ca;
    int8_t rank, data_type;
    ca_size_t dim[CA_RANK_MAX];
    size_t len;
    int i;

    data_type = rb_nc_typemap (type);
    rank = ndims;
    for (i=0; i<rank; i++) {
      status = nc_inq_dimlen(NUM2LONG(argv[0]), dimid[i], &len);
      CHECK_STATUS(status);
      dim[i] = len;
    }

    out = rb_carray_new(data_type, rank, dim, 0, NULL);
    Data_Get_Struct(out, CArray, ca);

    status = nc_get_var_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				type, ca->ptr);

    CHECK_STATUS(status);
  
    return out;
  }
  else {
    volatile VALUE data = argv[2];
    CArray *ca;

    if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
      rb_raise(rb_eTypeError, "arg3 must be a CArray object");
    }

    Data_Get_Struct(data, CArray, ca);

    type = rb_nc_rtypemap(ca->data_type);

    ca_attach(ca);
    status = nc_get_var_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				type, ca->ptr);
    ca_sync(ca);
    ca_detach(ca);

    CHECK_STATUS(status);
  
    return LONG2NUM(status);
  }
}

static VALUE
rb_nc_put_var (int argc, VALUE *argv, VALUE mod)
{
  volatile VALUE data;
  int status;
  nc_type type;
  CArray *ca;

  CHECK_ARGC(3);
  
  status = nc_inq_vartype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &type);

  CHECK_STATUS(status);

  data = argv[2];

  if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
    rb_raise(rb_eTypeError, "arg3 must be a CArray object");
  }

  Data_Get_Struct(data, CArray, ca);

  type = rb_nc_rtypemap(ca->data_type);
  ca_attach(ca);
  status = nc_put_var_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			      type, ca->ptr);
  ca_detach(ca);

  CHECK_STATUS(status);
  
  return LONG2NUM(status);
}

static VALUE
rb_nc_get_vara (int argc, VALUE *argv, VALUE mod)
{
  int status;
  nc_type type;
  int ndims;
  int dimid[NC_MAX_DIMS];
  size_t start[NC_MAX_DIMS], count[NC_MAX_DIMS];
  int i;

  if ( argc < 4 ) {
    rb_raise(rb_eArgError, "invalid # of arguments");
  }
  
  status = nc_inq_vartype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &type);

  CHECK_STATUS(status);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  status = nc_inq_vardimid(NUM2LONG(argv[0]), NUM2LONG(argv[1]), dimid);

  CHECK_STATUS(status);

  Check_Type(argv[2], T_ARRAY);
  Check_Type(argv[3], T_ARRAY);

  for (i=0; i<ndims; i++) {
    start[i] = NUM2ULONG(RARRAY_PTR(argv[2])[i]);
    count[i] = NUM2ULONG(RARRAY_PTR(argv[3])[i]);
  }

  if ( argc == 4 ) {
    volatile VALUE out;
    CArray *ca;
    int8_t rank, data_type;
    ca_size_t dim[CA_RANK_MAX];
    int i;

    data_type = rb_nc_typemap (type);
    rank = ndims;
    for (i=0; i<rank; i++) {
      dim[i] = count[i];
    }

    out = rb_carray_new(data_type, rank, dim, 0, NULL);
    Data_Get_Struct(out, CArray, ca);

    status = nc_get_vara_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				 type, start, count, ca->ptr);

    CHECK_STATUS(status);
  
    return out;
  }
  else {
    volatile VALUE data = argv[4];
    CArray *ca;

    if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
      rb_raise(rb_eTypeError, "arg5 must be a CArray object");
    }
    Data_Get_Struct(data, CArray, ca);

    if ( ca->rank != ndims ) {
      rb_raise(rb_eRuntimeError, "rank mismatch");
    }

    for (i=0; i<ca->rank; i++) {
      if ( ca->dim[i] != count[i] ) {
	rb_raise(rb_eRuntimeError, "dim[%i] mismatch", i);
      }
    }

    type = rb_nc_rtypemap(ca->data_type);

    ca_attach(ca);
    status = nc_get_vara_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, start, count, ca->ptr);
    ca_sync(ca);
    ca_detach(ca);

    CHECK_STATUS(status);
  
    return LONG2NUM(status);
  }
}

static VALUE
rb_nc_put_vara (int argc, VALUE *argv, VALUE mod)
{
  volatile VALUE data;
  int status;
  nc_type type;
  int ndims;
  size_t start[NC_MAX_DIMS], count[NC_MAX_DIMS];
  CArray *ca;
  int i;

  CHECK_ARGC(5);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  Check_Type(argv[2], T_ARRAY);
  Check_Type(argv[3], T_ARRAY);

  for (i=0; i<ndims; i++) {
    start[i] = NUM2ULONG(RARRAY_PTR(argv[2])[i]);
    count[i] = NUM2ULONG(RARRAY_PTR(argv[3])[i]);
  }

  data = argv[4];

  if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
    rb_raise(rb_eTypeError, "arg5 must be a CArray object");
  }

  Data_Get_Struct(data, CArray, ca);

  type = rb_nc_rtypemap(ca->data_type);

  ca_attach(ca);
  status = nc_put_vara_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, start, count, ca->ptr);
  ca_detach(ca);

  CHECK_STATUS(status);
  
  return LONG2NUM(status);
}

static VALUE
rb_nc_get_vars (int argc, VALUE *argv, VALUE mod)
{
  int status;
  nc_type type;
  int ndims;
  int dimid[NC_MAX_DIMS];
  size_t start[NC_MAX_DIMS], count[NC_MAX_DIMS];
  ptrdiff_t stride[NC_MAX_DIMS];
  CArray *ca;
  int i;

  if ( argc < 5 ) {
    rb_raise(rb_eArgError, "invalid # of arguments");
  }
  
  status = nc_inq_vartype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &type);

  CHECK_STATUS(status);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  status = nc_inq_vardimid(NUM2LONG(argv[0]), NUM2LONG(argv[1]), dimid);

  CHECK_STATUS(status);

  Check_Type(argv[2], T_ARRAY);
  Check_Type(argv[3], T_ARRAY);
  Check_Type(argv[4], T_ARRAY);

  for (i=0; i<ndims; i++) {
    start[i]  = NUM2ULONG(RARRAY_PTR(argv[2])[i]);
    count[i]  = NUM2ULONG(RARRAY_PTR(argv[3])[i]);
    stride[i] = NUM2ULONG(RARRAY_PTR(argv[4])[i]);
  }

  if ( argc == 5 ) {
    volatile VALUE out;
    CArray *ca;
    int8_t rank, data_type;
    ca_size_t dim[CA_RANK_MAX];
    int i;

    data_type = rb_nc_typemap (type);
    rank = ndims;
    for (i=0; i<rank; i++) {
      dim[i] = count[i];
    }

    out = rb_carray_new(data_type, rank, dim, 0, NULL);
    Data_Get_Struct(out, CArray, ca);

    status = nc_get_vars_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
				 type, start, count, stride, ca->ptr);

    CHECK_STATUS(status);
  
    return out;
  }
  else {
    volatile VALUE data = argv[5];
    int i;

    if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
      rb_raise(rb_eTypeError, "arg6 must be a CArray object");
    }

    Data_Get_Struct(data, CArray, ca);

    if ( ca->rank != ndims ) {
      rb_raise(rb_eRuntimeError, "rank mismatch");
    }

    for (i=0; i<ca->rank; i++) {
      if ( ca->dim[i] != count[i] ) {
	rb_raise(rb_eRuntimeError, "dim[%i] mismatch", i);
      }
    }

    type = rb_nc_rtypemap(ca->data_type);

    ca_attach(ca);
    status = nc_get_vars_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, start, count, stride, ca->ptr);
    ca_sync(ca);
    ca_detach(ca);

    CHECK_STATUS(status);
  
    return LONG2NUM(status);
  }
}

static VALUE
rb_nc_put_vars (int argc, VALUE *argv, VALUE mod)
{
  volatile VALUE data;
  int status;
  nc_type type;
  int ndims;
  size_t start[NC_MAX_DIMS], count[NC_MAX_DIMS];
  ptrdiff_t stride[NC_MAX_DIMS];
  CArray *ca;
  int i;

  CHECK_ARGC(6);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  Check_Type(argv[2], T_ARRAY);
  Check_Type(argv[3], T_ARRAY);
  Check_Type(argv[4], T_ARRAY);

  for (i=0; i<ndims; i++) {
    start[i]  = NUM2ULONG(RARRAY_PTR(argv[2])[i]);
    count[i]  = NUM2ULONG(RARRAY_PTR(argv[3])[i]);
    stride[i] = NUM2ULONG(RARRAY_PTR(argv[4])[i]);
  }

  data = argv[5];

  if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
    rb_raise(rb_eTypeError, "arg6 must be a CArray object");
  }

  Data_Get_Struct(data, CArray, ca);

  type = rb_nc_rtypemap(ca->data_type);

  ca_attach(ca);
  status = nc_put_vars_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, start, count, stride, ca->ptr);
  ca_detach(ca);

  CHECK_STATUS(status);
  
  return LONG2NUM(status);
}

static VALUE
rb_nc_get_varm (int argc, VALUE *argv, VALUE mod)
{
  int status;
  nc_type type;
  int ndims;
  int dimid[NC_MAX_DIMS];
  size_t    start[NC_MAX_DIMS], count[NC_MAX_DIMS];
  ptrdiff_t stride[NC_MAX_DIMS], imap[NC_MAX_DIMS];
  CArray *ca;
  int i;

  if ( argc < 6 ) {
    rb_raise(rb_eArgError, "invalid # of arguments");
  }
  
  status = nc_inq_vartype(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &type);

  CHECK_STATUS(status);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  status = nc_inq_vardimid(NUM2LONG(argv[0]), NUM2LONG(argv[1]), dimid);

  CHECK_STATUS(status);

  Check_Type(argv[2], T_ARRAY);
  Check_Type(argv[3], T_ARRAY);
  Check_Type(argv[4], T_ARRAY);
  Check_Type(argv[5], T_ARRAY);

  for (i=0; i<ndims; i++) {
    start[i]  = NUM2ULONG(RARRAY_PTR(argv[2])[i]);
    count[i]  = NUM2ULONG(RARRAY_PTR(argv[3])[i]);
    stride[i] = NUM2ULONG(RARRAY_PTR(argv[4])[i]);
    imap[i]   = NUM2ULONG(RARRAY_PTR(argv[5])[i]);
  }

  if ( argc == 6 ) {
    volatile VALUE out;
    CArray *ca;
    int8_t rank, data_type;
    ca_size_t dim[CA_RANK_MAX];
    int i;

    data_type = rb_nc_typemap (type);
    rank = ndims;
    for (i=0; i<rank; i++) {
      dim[i] = count[i];
    }

    out = rb_carray_new(data_type, rank, dim, 0, NULL);
    Data_Get_Struct(out, CArray, ca);

    status = nc_get_varm_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, start, count, stride, imap, ca->ptr);

    CHECK_STATUS(status);
  
    return out;
  }
  else {
    volatile VALUE data = argv[6];
    int i;

    if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
      rb_raise(rb_eTypeError, "arg6 must be a CArray object");
    }

    Data_Get_Struct(data, CArray, ca);

    if ( ca->rank != ndims ) {
      rb_raise(rb_eRuntimeError, "rank mismatch");
    }

    for (i=0; i<ca->rank; i++) {
      if ( ca->dim[i] != count[i] ) {
	rb_raise(rb_eRuntimeError, "dim[%i] mismatch", i);
      }
    }

    type = rb_nc_rtypemap(ca->data_type);

    ca_attach(ca);
    status = nc_get_varm_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, start, count, stride, imap, ca->ptr);
    ca_sync(ca);
    ca_detach(ca);
  
    CHECK_STATUS(status);
  
    return LONG2NUM(status);
  }
}

static VALUE
rb_nc_put_varm (int argc, VALUE *argv, VALUE mod)
{
  volatile VALUE data;
  int status;
  nc_type type;
  int ndims;
  size_t start[NC_MAX_DIMS], count[NC_MAX_DIMS];
  ptrdiff_t stride[NC_MAX_DIMS], imap[NC_MAX_DIMS];
  CArray *ca;
  int i;

  CHECK_ARGC(7);

  status = nc_inq_varndims(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &ndims);

  CHECK_STATUS(status);

  Check_Type(argv[2], T_ARRAY);
  Check_Type(argv[3], T_ARRAY);
  Check_Type(argv[4], T_ARRAY);
  Check_Type(argv[5], T_ARRAY);

  for (i=0; i<ndims; i++) {
    start[i]  = NUM2ULONG(RARRAY_PTR(argv[2])[i]);
    count[i]  = NUM2ULONG(RARRAY_PTR(argv[3])[i]);
    stride[i] = NUM2ULONG(RARRAY_PTR(argv[4])[i]);
    imap[i]   = NUM2ULONG(RARRAY_PTR(argv[5])[i]);
  }

  data = argv[6];

  if ( ! rb_obj_is_kind_of(data, rb_cCArray) ) {
    rb_raise(rb_eTypeError, "arg7 must be a CArray object");
  }

  Data_Get_Struct(data, CArray, ca);

  type = rb_nc_rtypemap(ca->data_type);

  ca_attach(ca);
  status = nc_put_varm_numeric(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			       type, start, count, stride, imap, ca->ptr);
  ca_detach(ca);

  CHECK_STATUS(status);
  
  return LONG2NUM(status);
}

static VALUE
rb_nc_rename_dim (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(3);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);

  status = nc_rename_dim(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			 StringValuePtr(argv[2]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

static VALUE
rb_nc_rename_var (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(3);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);
  
  status = nc_rename_var(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			 StringValuePtr(argv[2]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

static VALUE
rb_nc_rename_att (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(4);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);
  CHECK_TYPE_STRING(argv[3]);
  
  status = nc_rename_att(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
			 StringValuePtr(argv[2]), StringValuePtr(argv[3]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

int nc_setfill(int ncid, int fillmode, int* old_fillemode);

static VALUE
rb_nc_setfill (int argc, VALUE *argv, VALUE mod)
{
  int status;
  int old_fillmode;

  CHECK_ARGC(2);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_INT(argv[1]);

  status = nc_setfill(NUM2LONG(argv[0]), NUM2LONG(argv[1]), &old_fillmode);

  CHECK_STATUS(status);

  return LONG2NUM(old_fillmode);
}

static VALUE
rb_nc_copy_att (int argc, VALUE *argv, VALUE mod)
{
  int status;

  CHECK_ARGC(5);
  CHECK_TYPE_ID(argv[0]);
  CHECK_TYPE_ID(argv[1]);
  CHECK_TYPE_STRING(argv[2]);
  CHECK_TYPE_ID(argv[3]);
  CHECK_TYPE_ID(argv[4]);
  
  status = nc_copy_att(NUM2LONG(argv[0]), NUM2LONG(argv[1]), 
		      StringValuePtr(argv[2]), NUM2LONG(argv[3]), NUM2LONG(argv[4]));

  CHECK_STATUS(status);

  return LONG2NUM(status);
}

void
Init_netcdflib ()
{

  mNetCDF = rb_define_module("NC");

  rb_define_singleton_method(mNetCDF, "ca_type",  rb_nc_ca_type, -1);
  rb_define_singleton_method(mNetCDF, "nc_type",  rb_nc_nc_type, -1);

  rb_define_module_function(mNetCDF, "nc_create", rb_nc_create, -1);
  rb_define_singleton_method(mNetCDF,   "create", rb_nc_create, -1);
  rb_define_module_function(mNetCDF, "nc_open",   rb_nc_open, -1);
  rb_define_singleton_method(mNetCDF,   "open",   rb_nc_open, -1);
  rb_define_module_function(mNetCDF, "nc_close",  rb_nc_close, -1);
  rb_define_singleton_method(mNetCDF,   "close",  rb_nc_close, -1);
  rb_define_module_function(mNetCDF, "nc_redef",  rb_nc_redef, -1);
  rb_define_singleton_method(mNetCDF,   "redef",  rb_nc_redef, -1);
  rb_define_module_function(mNetCDF, "nc_enddef", rb_nc_enddef, -1);
  rb_define_singleton_method(mNetCDF,   "enddef", rb_nc_enddef, -1);
  rb_define_module_function(mNetCDF, "nc_sync",   rb_nc_sync, -1);
  rb_define_singleton_method(mNetCDF,   "sync",   rb_nc_sync, -1);

  rb_define_module_function(mNetCDF, "nc_inq_ndims",   rb_nc_inq_ndims, -1);
  rb_define_singleton_method(mNetCDF,   "inq_ndims",   rb_nc_inq_ndims, -1);
  rb_define_module_function(mNetCDF, "nc_inq_nvars",   rb_nc_inq_nvars, -1);
  rb_define_singleton_method(mNetCDF,   "inq_nvars",   rb_nc_inq_nvars, -1);
  rb_define_module_function(mNetCDF, "nc_inq_natts",   rb_nc_inq_natts, -1);
  rb_define_singleton_method(mNetCDF,   "inq_natts",   rb_nc_inq_natts, -1);
  rb_define_module_function(mNetCDF, "nc_inq_unlimdim",   rb_nc_inq_unlimdim, -1);
  rb_define_singleton_method(mNetCDF,   "inq_unlimdim",   rb_nc_inq_unlimdim, -1);

  rb_define_module_function(mNetCDF, "nc_inq_dimid",   rb_nc_inq_dimid, -1);
  rb_define_singleton_method(mNetCDF,   "inq_dimid",   rb_nc_inq_dimid, -1);
  rb_define_module_function(mNetCDF, "nc_inq_varid",   rb_nc_inq_varid, -1);
  rb_define_singleton_method(mNetCDF,   "inq_varid",   rb_nc_inq_varid, -1);
  rb_define_module_function(mNetCDF, "nc_inq_attid",   rb_nc_inq_attid, -1);
  rb_define_singleton_method(mNetCDF,   "inq_attid",   rb_nc_inq_attid, -1);

  rb_define_module_function(mNetCDF, "nc_inq_dimlen",  rb_nc_inq_dimlen, -1);
  rb_define_singleton_method(mNetCDF,   "inq_dimlen",  rb_nc_inq_dimlen, -1);
  rb_define_module_function(mNetCDF, "nc_inq_dimname", rb_nc_inq_dimname, -1);
  rb_define_singleton_method(mNetCDF,   "inq_dimname", rb_nc_inq_dimname, -1);

  rb_define_module_function(mNetCDF, "nc_inq_varname",    rb_nc_inq_varname, -1);
  rb_define_singleton_method(mNetCDF,   "inq_varname",    rb_nc_inq_varname, -1);
  rb_define_module_function(mNetCDF, "nc_inq_vartype",    rb_nc_inq_vartype, -1);
  rb_define_singleton_method(mNetCDF,   "inq_vartype",    rb_nc_inq_vartype, -1);
  rb_define_module_function(mNetCDF, "nc_inq_varndims",   rb_nc_inq_varndims, -1);
  rb_define_singleton_method(mNetCDF,   "inq_varndims",   rb_nc_inq_varndims, -1);
  rb_define_module_function(mNetCDF, "nc_inq_vardimid",  rb_nc_inq_vardimid, -1);
  rb_define_singleton_method(mNetCDF,    "inq_vardimid",  rb_nc_inq_vardimid, -1);
  rb_define_module_function(mNetCDF, "nc_inq_varnatts",   rb_nc_inq_varnatts, -1);
  rb_define_singleton_method(mNetCDF,    "inq_varnatts",  rb_nc_inq_varnatts, -1);

  rb_define_module_function(mNetCDF, "nc_inq_attname", rb_nc_inq_attname, -1);
  rb_define_singleton_method(mNetCDF,   "inq_attname", rb_nc_inq_attname, -1);
  rb_define_module_function(mNetCDF, "nc_inq_atttype", rb_nc_inq_atttype, -1);
  rb_define_singleton_method(mNetCDF,   "inq_atttype", rb_nc_inq_atttype, -1);
  rb_define_module_function(mNetCDF, "nc_inq_attlen",  rb_nc_inq_attlen, -1);
  rb_define_singleton_method(mNetCDF,   "inq_attlen",  rb_nc_inq_attlen, -1);
  rb_define_module_function(mNetCDF, "nc_inq_attid",   rb_nc_inq_attid, -1);
  rb_define_singleton_method(mNetCDF,   "inq_attid",   rb_nc_inq_attid, -1);

  rb_define_module_function(mNetCDF, "nc_def_dim",  rb_nc_def_dim, -1);
  rb_define_singleton_method(mNetCDF,   "def_dim",  rb_nc_def_dim, -1);
  rb_define_module_function(mNetCDF, "nc_def_var",  rb_nc_def_var, -1);
  rb_define_singleton_method(mNetCDF,   "def_var",  rb_nc_def_var, -1);
  rb_define_module_function(mNetCDF, "nc_rename_dim",  rb_nc_rename_dim, -1);
  rb_define_singleton_method(mNetCDF,   "rename_dim",  rb_nc_rename_dim, -1);
  rb_define_module_function(mNetCDF, "nc_rename_var",  rb_nc_rename_var, -1);
  rb_define_singleton_method(mNetCDF,   "rename_var",  rb_nc_rename_var, -1);
  rb_define_module_function(mNetCDF, "nc_rename_att",  rb_nc_rename_att, -1);
  rb_define_singleton_method(mNetCDF,   "rename_att",  rb_nc_rename_att, -1);
  rb_define_module_function(mNetCDF, "nc_del_att",  rb_nc_del_att, -1);
  rb_define_singleton_method(mNetCDF,   "del_att",  rb_nc_del_att, -1);
  rb_define_module_function(mNetCDF, "nc_setfill",  rb_nc_setfill, -1);
  rb_define_singleton_method(mNetCDF,   "setfill",  rb_nc_setfill, -1);

  rb_define_module_function(mNetCDF, "nc_put_att",  rb_nc_put_att, -1);
  rb_define_singleton_method(mNetCDF,   "put_att",  rb_nc_put_att, -1);
  rb_define_module_function(mNetCDF, "nc_get_att",  rb_nc_get_att, -1);
  rb_define_singleton_method(mNetCDF,   "get_att",  rb_nc_get_att, -1);
  rb_define_module_function(mNetCDF, "nc_copy_att", rb_nc_copy_att, -1);
  rb_define_singleton_method(mNetCDF,   "copy_att", rb_nc_copy_att, -1);

  rb_define_module_function(mNetCDF, "nc_get_var1", rb_nc_get_var1, -1);
  rb_define_singleton_method(mNetCDF,   "get_var1", rb_nc_get_var1, -1);
  rb_define_module_function(mNetCDF, "nc_put_var1", rb_nc_put_var1, -1);
  rb_define_singleton_method(mNetCDF,   "put_var1", rb_nc_put_var1, -1);
  rb_define_module_function(mNetCDF, "nc_get_var",  rb_nc_get_var, -1);
  rb_define_singleton_method(mNetCDF,   "get_var",  rb_nc_get_var, -1);
  rb_define_module_function(mNetCDF, "nc_put_var",  rb_nc_put_var, -1);
  rb_define_singleton_method(mNetCDF,   "put_var",  rb_nc_put_var, -1);
  rb_define_module_function(mNetCDF, "nc_get_vara", rb_nc_get_vara, -1);
  rb_define_singleton_method(mNetCDF,   "get_vara", rb_nc_get_vara, -1);
  rb_define_module_function(mNetCDF, "nc_put_vara", rb_nc_put_vara, -1);
  rb_define_singleton_method(mNetCDF,   "put_vara", rb_nc_put_vara, -1);
  rb_define_module_function(mNetCDF, "nc_get_vars", rb_nc_get_vars, -1);
  rb_define_singleton_method(mNetCDF,   "get_vars", rb_nc_get_vars, -1);
  rb_define_module_function(mNetCDF, "nc_put_vars", rb_nc_put_vars, -1);
  rb_define_singleton_method(mNetCDF,   "put_vars", rb_nc_put_vars, -1);
  rb_define_module_function(mNetCDF, "nc_get_varm", rb_nc_get_varm, -1);
  rb_define_singleton_method(mNetCDF,   "get_varm", rb_nc_get_varm, -1);
  rb_define_module_function(mNetCDF, "nc_put_varm", rb_nc_put_varm, -1);
  rb_define_singleton_method(mNetCDF,   "put_varm", rb_nc_put_varm, -1);

  rb_define_const(mNetCDF, "NC_NOERR",     INT2FIX(NC_NOERR));

  rb_define_const(mNetCDF, "NC_NOWRITE",   INT2FIX(NC_NOWRITE));
  rb_define_const(mNetCDF, "NC_WRITE",     INT2FIX(NC_WRITE));
  rb_define_const(mNetCDF, "NC_SHARE",     INT2FIX(NC_SHARE));
  rb_define_const(mNetCDF, "NC_LOCK",      INT2FIX(NC_LOCK));
  rb_define_const(mNetCDF, "NC_CLOBBER",   INT2FIX(NC_CLOBBER));
  rb_define_const(mNetCDF, "NC_NOCLOBBER", INT2FIX(NC_NOCLOBBER));
  rb_define_const(mNetCDF, "NC_SIZEHINT_DEFAULT", INT2FIX(NC_SIZEHINT_DEFAULT));

  rb_define_const(mNetCDF, "NC_GLOBAL",       INT2FIX(NC_GLOBAL));
  rb_define_const(mNetCDF, "NC_MAX_NAME",     INT2FIX(NC_MAX_NAME));
  rb_define_const(mNetCDF, "NC_MAX_VAR_DIMS", INT2FIX(NC_MAX_VAR_DIMS));
  rb_define_const(mNetCDF, "NC_MAX_DIMS",     INT2FIX(NC_MAX_DIMS));
  rb_define_const(mNetCDF, "NC_NOFILL",       INT2FIX(NC_NOFILL));
  rb_define_const(mNetCDF, "NC_FILL",         INT2FIX(NC_FILL));

  rb_define_const(mNetCDF, "NC_NAT",     INT2FIX(NC_NAT));
  rb_define_const(mNetCDF, "NC_BYTE",    INT2FIX(NC_BYTE));
  rb_define_const(mNetCDF, "NC_CHAR",    INT2FIX(NC_CHAR));
  rb_define_const(mNetCDF, "NC_SHORT",   INT2FIX(NC_SHORT));
  rb_define_const(mNetCDF, "NC_INT",     INT2FIX(NC_INT));
  rb_define_const(mNetCDF, "NC_FLOAT",   INT2FIX(NC_FLOAT));
  rb_define_const(mNetCDF, "NC_DOUBLE",  INT2FIX(NC_DOUBLE));

}
