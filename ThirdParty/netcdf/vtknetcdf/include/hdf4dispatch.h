/* Copyright 2018, UCAR/Unidata. See netcdf/COPYRIGHT file for copying
 * and redistribution conditions. */
/**
 * @file
 * This header file contains the prototypes for the HDF4 versions
 * of the netCDF functions.
 *
 * Ed Hartnett
 */
#ifndef _HDF4DISPATCH_H
#define _HDF4DISPATCH_H

#include "config.h"
#include <stddef.h> /* size_t, ptrdiff_t */
#include <errno.h>  /* netcdf functions sometimes return system errors */
#include "ncdispatch.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern int
HDF4_create(const char *path, int cmode,
           size_t initialsz, int basepe, size_t *chunksizehintp,
	   int useparallel, void* parameters,
	   NC_Dispatch*, NC*);

extern int
HDF4_open(const char *path, int mode,
         int basepe, size_t *chunksizehintp, 
	 int use_parallel, void* parameters,
	 NC_Dispatch*, NC*);

extern int
HDF4_redef(int ncid);

extern int
HDF4__enddef(int ncid, size_t h_minfree, size_t v_align,
	size_t v_minfree, size_t r_align);

extern int
HDF4_sync(int ncid);

extern int
HDF4_abort(int ncid);

extern int
HDF4_close(int ncid);

extern int
HDF4_set_fill(int ncid, int fillmode, int *old_modep);

extern int
HDF4_set_base_pe(int ncid, int pe);

extern int
HDF4_inq_base_pe(int ncid, int *pe);

extern int
HDF4_inq_format(int ncid, int *formatp);

extern int
HDF4_inq_format_extended(int ncid, int *formatp, int *modep);

extern int
HDF4_inq(int ncid, int *ndimsp, int *nvarsp, int *nattsp, int *unlimdimidp);

extern int
HDF4_inq_type(int, nc_type, char *, size_t *);

/* Begin _dim */

extern int
HDF4_def_dim(int ncid, const char *name, size_t len, int *idp);

extern int
HDF4_inq_dimid(int ncid, const char *name, int *idp);

extern int
HDF4_inq_dim(int ncid, int dimid, char *name, size_t *lenp);

extern int
HDF4_inq_unlimdim(int ncid, int *unlimdimidp);

extern int
HDF4_rename_dim(int ncid, int dimid, const char *name);

/* End _dim */
/* Begin _att */

extern int
HDF4_inq_att(int ncid, int varid, const char *name,
	    nc_type *xtypep, size_t *lenp);

extern int 
HDF4_inq_attid(int ncid, int varid, const char *name, int *idp);

extern int
HDF4_inq_attname(int ncid, int varid, int attnum, char *name);

extern int
HDF4_rename_att(int ncid, int varid, const char *name, const char *newname);

extern int
HDF4_del_att(int ncid, int varid, const char*);

/* End _att */
/* Begin {put,get}_att */

extern int
HDF4_get_att(int ncid, int varid, const char *name, void *value, nc_type);

extern int
HDF4_put_att(int ncid, int varid, const char *name, nc_type datatype,
	   size_t len, const void *value, nc_type);

/* End {put,get}_att */
/* Begin _var */

extern int
HDF4_def_var(int ncid, const char *name,
	 nc_type xtype, int ndims, const int *dimidsp, int *varidp);

extern int
HDF4_inq_var_all(int ncid, int varid, char *name, nc_type *xtypep, 
               int *ndimsp, int *dimidsp, int *nattsp, 
               int *shufflep, int *deflatep, int *deflate_levelp,
               int *fletcher32p, int *contiguousp, size_t *chunksizesp, 
               int *no_fill, void *fill_valuep, int *endiannessp, 
	       unsigned int* idp, size_t* nparamsp, unsigned int* params
	       );

extern int
HDF4_inq_varid(int ncid, const char *name, int *varidp);

extern int
HDF4_rename_var(int ncid, int varid, const char *name);

extern int
HDF4_put_vara(int ncid, int varid,
   	     const size_t *start, const size_t *count,
             const void *value, nc_type);

extern int
HDF4_get_vara(int ncid, int varid,
	     const size_t *start, const size_t *count,
             void *value, nc_type);

/* End _var */

/* netCDF4 API only */
extern int
HDF4_var_par_access(int, int, int);

extern int
HDF4_inq_ncid(int, const char *, int *);

extern int
HDF4_inq_grps(int, int *, int *);

extern int
HDF4_inq_grpname(int, char *);

extern int
HDF4_inq_grpname_full(int, size_t *, char *);

extern int
HDF4_inq_grp_parent(int, int *);

extern int
HDF4_inq_grp_full_ncid(int, const char *, int *);

extern int
HDF4_inq_varids(int, int * nvars, int *);

extern int
HDF4_inq_dimids(int, int * ndims, int *, int);

extern int
HDF4_inq_typeids(int, int * ntypes, int *);
   
extern int
HDF4_inq_type_equal(int, nc_type, int, nc_type, int *);

extern int
HDF4_def_grp(int, const char *, int *);

extern int
HDF4_rename_grp(int, const char *);

extern int
HDF4_inq_user_type(int, nc_type, char *, size_t *, nc_type *, 
		  size_t *, int *);

extern int
HDF4_def_compound(int, size_t, const char *, nc_type *);

extern int
HDF4_insert_compound(int, nc_type, const char *, size_t, nc_type);

extern int
HDF4_insert_array_compound(int, nc_type, const char *, size_t, 
			  nc_type, int, const int *);

extern int
HDF4_inq_typeid(int, const char *, nc_type *);

extern int
HDF4_inq_compound_field(int, nc_type, int, char *, size_t *, 
		       nc_type *, int *, int *);

extern int
HDF4_inq_compound_fieldindex(int, nc_type, const char *, int *);

extern int
HDF4_def_vlen(int, const char *, nc_type base_typeid, nc_type *);

extern int
HDF4_put_vlen_element(int, int, void *, size_t, const void *);

extern int
HDF4_get_vlen_element(int, int, const void *, size_t *, void *);

extern int
HDF4_def_enum(int, nc_type, const char *, nc_type *);

extern int
HDF4_insert_enum(int, nc_type, const char *, const void *);

extern int
HDF4_inq_enum_member(int, nc_type, int, char *, void *);

extern int
HDF4_inq_enum_ident(int, nc_type, long long, char *);

extern int
HDF4_def_opaque(int, size_t, const char *, nc_type *);

extern int
HDF4_def_var_deflate(int, int, int, int, int);

extern int
HDF4_def_var_fletcher32(int, int, int);

extern int
HDF4_def_var_chunking(int, int, int, const size_t *);

extern int
HDF4_def_var_fill(int, int, int, const void *);

extern int
HDF4_def_var_endian(int, int, int);

extern int
HDF4_def_var_filter(int, int, unsigned int, size_t, const unsigned int*);

extern int
HDF4_set_var_chunk_cache(int, int, size_t, size_t, float);

extern int
HDF4_get_var_chunk_cache(int, int, size_t *, size_t *, float *);

extern int
HDF4_inq_unlimdims(int, int *, int *);

extern int
HDF4_show_metadata(int);

extern int 
HDF4_initialize(void);

#if defined(__cplusplus)
}
#endif

#endif /*_HDF4DISPATCH_H */
