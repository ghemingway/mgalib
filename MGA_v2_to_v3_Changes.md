# Introduction #

The MGA file format has not changed in quite a while.  This has been good, but it is time for a change.  Listed below are the proposed changes at both the [BinFile](http://code.google.com/p/mgalib/source/browse/trunk/Core/BinFile.cpp?r=6) level and as will be incurred by changing [MgaGeneric](http://code.google.com/p/mgalib/source/browse/trunk/MGA/MgaGeneric.cpp?r=5) and [MetaGeneric](http://code.google.com/p/mgalib/source/browse/trunk/Meta/MetaGeneric.cpp?r=5).

Any of these changes may impact the actual binary format of the MGA file.  An attempt at a complete description of the v3 MGA file format can be found at [MGAv3\_Binary\_File\_Format](MGAv3_Binary_File_Format.md).


# Proposed Changes #

Add your content here.  Format your content with:
  * Add index of all objects to front of file (GUID, file start position, object size in bytes)
  * Add back-pointer collection data
  * Replace all object identification schemes with GUID
  * Remove all lock attributes