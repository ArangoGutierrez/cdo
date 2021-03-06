# This file was automatically generated by SWIG (http://www.swig.org).
# Version 1.3.36
#
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _CdiLib
import new
new_instancemethod = new.instancemethod
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'PySwigObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


CDI_UNDEFID = _CdiLib.CDI_UNDEFID
CDI_GLOBAL = _CdiLib.CDI_GLOBAL
CDI_BIGENDIAN = _CdiLib.CDI_BIGENDIAN
CDI_LITTLEENDIAN = _CdiLib.CDI_LITTLEENDIAN
CDI_NOERR = _CdiLib.CDI_NOERR
CDI_ESYSTEM = _CdiLib.CDI_ESYSTEM
CDI_EINVAL = _CdiLib.CDI_EINVAL
CDI_EUFTYPE = _CdiLib.CDI_EUFTYPE
CDI_ELIBNAVAIL = _CdiLib.CDI_ELIBNAVAIL
CDI_EUFSTRUCT = _CdiLib.CDI_EUFSTRUCT
CDI_EUNC4 = _CdiLib.CDI_EUNC4
FILETYPE_GRB = _CdiLib.FILETYPE_GRB
FILETYPE_GRB2 = _CdiLib.FILETYPE_GRB2
FILETYPE_NC = _CdiLib.FILETYPE_NC
FILETYPE_NC2 = _CdiLib.FILETYPE_NC2
FILETYPE_NC4 = _CdiLib.FILETYPE_NC4
FILETYPE_SRV = _CdiLib.FILETYPE_SRV
FILETYPE_EXT = _CdiLib.FILETYPE_EXT
FILETYPE_IEG = _CdiLib.FILETYPE_IEG
COMPRESS_NONE = _CdiLib.COMPRESS_NONE
COMPRESS_SZIP = _CdiLib.COMPRESS_SZIP
COMPRESS_GZIP = _CdiLib.COMPRESS_GZIP
COMPRESS_BZIP2 = _CdiLib.COMPRESS_BZIP2
COMPRESS_ZIP = _CdiLib.COMPRESS_ZIP
DATATYPE_PACK = _CdiLib.DATATYPE_PACK
DATATYPE_PACK1 = _CdiLib.DATATYPE_PACK1
DATATYPE_PACK2 = _CdiLib.DATATYPE_PACK2
DATATYPE_PACK3 = _CdiLib.DATATYPE_PACK3
DATATYPE_PACK4 = _CdiLib.DATATYPE_PACK4
DATATYPE_PACK5 = _CdiLib.DATATYPE_PACK5
DATATYPE_PACK6 = _CdiLib.DATATYPE_PACK6
DATATYPE_PACK7 = _CdiLib.DATATYPE_PACK7
DATATYPE_PACK8 = _CdiLib.DATATYPE_PACK8
DATATYPE_PACK9 = _CdiLib.DATATYPE_PACK9
DATATYPE_PACK10 = _CdiLib.DATATYPE_PACK10
DATATYPE_PACK11 = _CdiLib.DATATYPE_PACK11
DATATYPE_PACK12 = _CdiLib.DATATYPE_PACK12
DATATYPE_PACK13 = _CdiLib.DATATYPE_PACK13
DATATYPE_PACK14 = _CdiLib.DATATYPE_PACK14
DATATYPE_PACK15 = _CdiLib.DATATYPE_PACK15
DATATYPE_PACK16 = _CdiLib.DATATYPE_PACK16
DATATYPE_PACK17 = _CdiLib.DATATYPE_PACK17
DATATYPE_PACK18 = _CdiLib.DATATYPE_PACK18
DATATYPE_PACK19 = _CdiLib.DATATYPE_PACK19
DATATYPE_PACK20 = _CdiLib.DATATYPE_PACK20
DATATYPE_PACK21 = _CdiLib.DATATYPE_PACK21
DATATYPE_PACK22 = _CdiLib.DATATYPE_PACK22
DATATYPE_PACK23 = _CdiLib.DATATYPE_PACK23
DATATYPE_PACK24 = _CdiLib.DATATYPE_PACK24
DATATYPE_PACK25 = _CdiLib.DATATYPE_PACK25
DATATYPE_PACK26 = _CdiLib.DATATYPE_PACK26
DATATYPE_PACK27 = _CdiLib.DATATYPE_PACK27
DATATYPE_PACK28 = _CdiLib.DATATYPE_PACK28
DATATYPE_PACK29 = _CdiLib.DATATYPE_PACK29
DATATYPE_PACK30 = _CdiLib.DATATYPE_PACK30
DATATYPE_PACK31 = _CdiLib.DATATYPE_PACK31
DATATYPE_PACK32 = _CdiLib.DATATYPE_PACK32
DATATYPE_FLT32 = _CdiLib.DATATYPE_FLT32
DATATYPE_FLT64 = _CdiLib.DATATYPE_FLT64
DATATYPE_INT8 = _CdiLib.DATATYPE_INT8
DATATYPE_INT16 = _CdiLib.DATATYPE_INT16
DATATYPE_INT32 = _CdiLib.DATATYPE_INT32
DATATYPE_UINT8 = _CdiLib.DATATYPE_UINT8
DATATYPE_UINT16 = _CdiLib.DATATYPE_UINT16
DATATYPE_UINT32 = _CdiLib.DATATYPE_UINT32
DATATYPE_INT = _CdiLib.DATATYPE_INT
DATATYPE_FLT = _CdiLib.DATATYPE_FLT
DATATYPE_TXT = _CdiLib.DATATYPE_TXT
GRID_GENERIC = _CdiLib.GRID_GENERIC
GRID_GAUSSIAN = _CdiLib.GRID_GAUSSIAN
GRID_GAUSSIAN_REDUCED = _CdiLib.GRID_GAUSSIAN_REDUCED
GRID_LONLAT = _CdiLib.GRID_LONLAT
GRID_SPECTRAL = _CdiLib.GRID_SPECTRAL
GRID_FOURIER = _CdiLib.GRID_FOURIER
GRID_GME = _CdiLib.GRID_GME
GRID_TRAJECTORY = _CdiLib.GRID_TRAJECTORY
GRID_UNSTRUCTURED = _CdiLib.GRID_UNSTRUCTURED
GRID_CURVILINEAR = _CdiLib.GRID_CURVILINEAR
GRID_LCC = _CdiLib.GRID_LCC
GRID_LCC2 = _CdiLib.GRID_LCC2
GRID_LAEA = _CdiLib.GRID_LAEA
GRID_SINUSOIDAL = _CdiLib.GRID_SINUSOIDAL
ZAXIS_SURFACE = _CdiLib.ZAXIS_SURFACE
ZAXIS_GENERIC = _CdiLib.ZAXIS_GENERIC
ZAXIS_HYBRID = _CdiLib.ZAXIS_HYBRID
ZAXIS_HYBRID_HALF = _CdiLib.ZAXIS_HYBRID_HALF
ZAXIS_PRESSURE = _CdiLib.ZAXIS_PRESSURE
ZAXIS_HEIGHT = _CdiLib.ZAXIS_HEIGHT
ZAXIS_DEPTH_BELOW_SEA = _CdiLib.ZAXIS_DEPTH_BELOW_SEA
ZAXIS_DEPTH_BELOW_LAND = _CdiLib.ZAXIS_DEPTH_BELOW_LAND
ZAXIS_ISENTROPIC = _CdiLib.ZAXIS_ISENTROPIC
ZAXIS_TRAJECTORY = _CdiLib.ZAXIS_TRAJECTORY
ZAXIS_ALTITUDE = _CdiLib.ZAXIS_ALTITUDE
ZAXIS_SIGMA = _CdiLib.ZAXIS_SIGMA
ZAXIS_MEANSEA = _CdiLib.ZAXIS_MEANSEA
TAXIS_ABSOLUTE = _CdiLib.TAXIS_ABSOLUTE
TAXIS_RELATIVE = _CdiLib.TAXIS_RELATIVE
TIME_CONSTANT = _CdiLib.TIME_CONSTANT
TIME_VARIABLE = _CdiLib.TIME_VARIABLE
TUNIT_SECOND = _CdiLib.TUNIT_SECOND
TUNIT_MINUTE = _CdiLib.TUNIT_MINUTE
TUNIT_HOUR = _CdiLib.TUNIT_HOUR
TUNIT_DAY = _CdiLib.TUNIT_DAY
TUNIT_MONTH = _CdiLib.TUNIT_MONTH
TUNIT_YEAR = _CdiLib.TUNIT_YEAR
TUNIT_QUARTER = _CdiLib.TUNIT_QUARTER
CALENDAR_STANDARD = _CdiLib.CALENDAR_STANDARD
CALENDAR_PROLEPTIC = _CdiLib.CALENDAR_PROLEPTIC
CALENDAR_360DAYS = _CdiLib.CALENDAR_360DAYS
CALENDAR_365DAYS = _CdiLib.CALENDAR_365DAYS
CALENDAR_366DAYS = _CdiLib.CALENDAR_366DAYS
CALENDAR_NONE = _CdiLib.CALENDAR_NONE
cdiStringError = _CdiLib.cdiStringError
cdiDebug = _CdiLib.cdiDebug
cdiLibraryVersion = _CdiLib.cdiLibraryVersion
cdiPrintVersion = _CdiLib.cdiPrintVersion
cdiDefMissval = _CdiLib.cdiDefMissval
cdiInqMissval = _CdiLib.cdiInqMissval
cdiDefGlobal = _CdiLib.cdiDefGlobal
streamOpenRead = _CdiLib.streamOpenRead
streamOpenWrite = _CdiLib.streamOpenWrite
streamOpenAppend = _CdiLib.streamOpenAppend
streamClose = _CdiLib.streamClose
streamSync = _CdiLib.streamSync
streamDefVlist = _CdiLib.streamDefVlist
streamInqVlist = _CdiLib.streamInqVlist
streamInqFiletype = _CdiLib.streamInqFiletype
streamDefByteorder = _CdiLib.streamDefByteorder
streamInqByteorder = _CdiLib.streamInqByteorder
streamDefZtype = _CdiLib.streamDefZtype
streamDefZlevel = _CdiLib.streamDefZlevel
streamInqZtype = _CdiLib.streamInqZtype
streamInqZlevel = _CdiLib.streamInqZlevel
streamDefTimestep = _CdiLib.streamDefTimestep
streamInqTimestep = _CdiLib.streamInqTimestep
streamFilename = _CdiLib.streamFilename
streamFilesuffix = _CdiLib.streamFilesuffix
streamNtsteps = _CdiLib.streamNtsteps
streamNvals = _CdiLib.streamNvals
streamReadVar = _CdiLib.streamReadVar
streamWriteVar = _CdiLib.streamWriteVar
streamReadVarSlice = _CdiLib.streamReadVarSlice
streamWriteVarSlice = _CdiLib.streamWriteVarSlice
streamInqRecord = _CdiLib.streamInqRecord
streamDefRecord = _CdiLib.streamDefRecord
streamReadRecord = _CdiLib.streamReadRecord
streamWriteRecord = _CdiLib.streamWriteRecord
streamCopyRecord = _CdiLib.streamCopyRecord
streamInqGinfo = _CdiLib.streamInqGinfo
vlistCreate = _CdiLib.vlistCreate
vlistDestroy = _CdiLib.vlistDestroy
vlistDuplicate = _CdiLib.vlistDuplicate
vlistCopy = _CdiLib.vlistCopy
vlistCopyFlag = _CdiLib.vlistCopyFlag
vlistClearFlag = _CdiLib.vlistClearFlag
vlistCat = _CdiLib.vlistCat
vlistMerge = _CdiLib.vlistMerge
vlistPrint = _CdiLib.vlistPrint
vlistNvars = _CdiLib.vlistNvars
vlistNgrids = _CdiLib.vlistNgrids
vlistNzaxis = _CdiLib.vlistNzaxis
vlistDefNtsteps = _CdiLib.vlistDefNtsteps
vlistNtsteps = _CdiLib.vlistNtsteps
vlistGridsizeMax = _CdiLib.vlistGridsizeMax
vlistGrid = _CdiLib.vlistGrid
vlistGridIndex = _CdiLib.vlistGridIndex
vlistChangeGridIndex = _CdiLib.vlistChangeGridIndex
vlistChangeGrid = _CdiLib.vlistChangeGrid
vlistZaxis = _CdiLib.vlistZaxis
vlistZaxisIndex = _CdiLib.vlistZaxisIndex
vlistChangeZaxisIndex = _CdiLib.vlistChangeZaxisIndex
vlistChangeZaxis = _CdiLib.vlistChangeZaxis
vlistNrecs = _CdiLib.vlistNrecs
vlistDefTaxis = _CdiLib.vlistDefTaxis
vlistInqTaxis = _CdiLib.vlistInqTaxis
vlistDefTable = _CdiLib.vlistDefTable
vlistInqTable = _CdiLib.vlistInqTable
vlistDefInstitut = _CdiLib.vlistDefInstitut
vlistInqInstitut = _CdiLib.vlistInqInstitut
vlistDefModel = _CdiLib.vlistDefModel
vlistInqModel = _CdiLib.vlistInqModel
vlistDefVar = _CdiLib.vlistDefVar
vlistChangeVarGrid = _CdiLib.vlistChangeVarGrid
vlistChangeVarZaxis = _CdiLib.vlistChangeVarZaxis
vlistInqVar = _CdiLib.vlistInqVar
vlistInqVarGrid = _CdiLib.vlistInqVarGrid
vlistInqVarZaxis = _CdiLib.vlistInqVarZaxis
vlistInqVarTime = _CdiLib.vlistInqVarTime
vlistDefVarZtype = _CdiLib.vlistDefVarZtype
vlistInqVarZtype = _CdiLib.vlistInqVarZtype
vlistDefVarZlevel = _CdiLib.vlistDefVarZlevel
vlistInqVarZlevel = _CdiLib.vlistInqVarZlevel
vlistDefVarCode = _CdiLib.vlistDefVarCode
vlistInqVarCode = _CdiLib.vlistInqVarCode
vlistDefVarDatatype = _CdiLib.vlistDefVarDatatype
vlistInqVarDatatype = _CdiLib.vlistInqVarDatatype
vlistDefVarInstitut = _CdiLib.vlistDefVarInstitut
vlistInqVarInstitut = _CdiLib.vlistInqVarInstitut
vlistDefVarModel = _CdiLib.vlistDefVarModel
vlistInqVarModel = _CdiLib.vlistInqVarModel
vlistDefVarTable = _CdiLib.vlistDefVarTable
vlistInqVarTable = _CdiLib.vlistInqVarTable
vlistDefVarName = _CdiLib.vlistDefVarName
vlistInqVarName = _CdiLib.vlistInqVarName
vlistDefVarLongname = _CdiLib.vlistDefVarLongname
vlistDefVarStdname = _CdiLib.vlistDefVarStdname
vlistInqVarLongname = _CdiLib.vlistInqVarLongname
vlistInqVarStdname = _CdiLib.vlistInqVarStdname
vlistDefVarUnits = _CdiLib.vlistDefVarUnits
vlistInqVarUnits = _CdiLib.vlistInqVarUnits
vlistDefVarMissval = _CdiLib.vlistDefVarMissval
vlistInqVarMissval = _CdiLib.vlistInqVarMissval
vlistDefVarScalefactor = _CdiLib.vlistDefVarScalefactor
vlistInqVarScalefactor = _CdiLib.vlistInqVarScalefactor
vlistDefVarAddoffset = _CdiLib.vlistDefVarAddoffset
vlistInqVarAddoffset = _CdiLib.vlistInqVarAddoffset
vlistDefVarTimave = _CdiLib.vlistDefVarTimave
vlistInqVarTimave = _CdiLib.vlistInqVarTimave
vlistDefVarTimaccu = _CdiLib.vlistDefVarTimaccu
vlistInqVarTimaccu = _CdiLib.vlistInqVarTimaccu
vlistInqVarSize = _CdiLib.vlistInqVarSize
vlistInqVarID = _CdiLib.vlistInqVarID
vlistDefIndex = _CdiLib.vlistDefIndex
vlistInqIndex = _CdiLib.vlistInqIndex
vlistDefFlag = _CdiLib.vlistDefFlag
vlistInqFlag = _CdiLib.vlistInqFlag
vlistFindVar = _CdiLib.vlistFindVar
vlistFindLevel = _CdiLib.vlistFindLevel
vlistMergedVar = _CdiLib.vlistMergedVar
vlistMergedLevel = _CdiLib.vlistMergedLevel
vlistInqNatts = _CdiLib.vlistInqNatts
vlistInqAtt = _CdiLib.vlistInqAtt
vlistDelAtt = _CdiLib.vlistDelAtt
vlistDefAttInt = _CdiLib.vlistDefAttInt
vlistDefAttFlt = _CdiLib.vlistDefAttFlt
vlistDefAttTxt = _CdiLib.vlistDefAttTxt
vlistInqAttInt = _CdiLib.vlistInqAttInt
vlistInqAttFlt = _CdiLib.vlistInqAttFlt
vlistInqAttTxt = _CdiLib.vlistInqAttTxt
gridName = _CdiLib.gridName
gridNamePtr = _CdiLib.gridNamePtr
gridCompress = _CdiLib.gridCompress
gridDefMask = _CdiLib.gridDefMask
gridInqMask = _CdiLib.gridInqMask
gridPrint = _CdiLib.gridPrint
gridSize = _CdiLib.gridSize
gridCreate = _CdiLib.gridCreate
gridDestroy = _CdiLib.gridDestroy
gridDuplicate = _CdiLib.gridDuplicate
gridInqType = _CdiLib.gridInqType
gridInqSize = _CdiLib.gridInqSize
gridDefXsize = _CdiLib.gridDefXsize
gridInqXsize = _CdiLib.gridInqXsize
gridDefYsize = _CdiLib.gridDefYsize
gridInqYsize = _CdiLib.gridInqYsize
gridDefXvals = _CdiLib.gridDefXvals
gridInqXvals = _CdiLib.gridInqXvals
gridDefYvals = _CdiLib.gridDefYvals
gridInqYvals = _CdiLib.gridInqYvals
gridDefXname = _CdiLib.gridDefXname
gridDefXlongname = _CdiLib.gridDefXlongname
gridDefXunits = _CdiLib.gridDefXunits
gridDefYname = _CdiLib.gridDefYname
gridDefYlongname = _CdiLib.gridDefYlongname
gridDefYunits = _CdiLib.gridDefYunits
gridInqXname = _CdiLib.gridInqXname
gridInqXlongname = _CdiLib.gridInqXlongname
gridInqXstdname = _CdiLib.gridInqXstdname
gridInqXunits = _CdiLib.gridInqXunits
gridInqYname = _CdiLib.gridInqYname
gridInqYlongname = _CdiLib.gridInqYlongname
gridInqYstdname = _CdiLib.gridInqYstdname
gridInqYunits = _CdiLib.gridInqYunits
gridDefPrec = _CdiLib.gridDefPrec
gridInqPrec = _CdiLib.gridInqPrec
gridInqXval = _CdiLib.gridInqXval
gridInqYval = _CdiLib.gridInqYval
gridInqXinc = _CdiLib.gridInqXinc
gridInqYinc = _CdiLib.gridInqYinc
gridIsCircular = _CdiLib.gridIsCircular
gridIsRotated = _CdiLib.gridIsRotated
gridInqXpole = _CdiLib.gridInqXpole
gridDefXpole = _CdiLib.gridDefXpole
gridInqYpole = _CdiLib.gridInqYpole
gridDefYpole = _CdiLib.gridDefYpole
gridInqAngle = _CdiLib.gridInqAngle
gridDefAngle = _CdiLib.gridDefAngle
gridDefTrunc = _CdiLib.gridDefTrunc
gridInqTrunc = _CdiLib.gridInqTrunc
gridInqGMEnd = _CdiLib.gridInqGMEnd
gridDefGMEnd = _CdiLib.gridDefGMEnd
gridInqGMEni = _CdiLib.gridInqGMEni
gridDefGMEni = _CdiLib.gridDefGMEni
gridInqGMEni2 = _CdiLib.gridInqGMEni2
gridDefGMEni2 = _CdiLib.gridDefGMEni2
gridInqGMEni3 = _CdiLib.gridInqGMEni3
gridDefGMEni3 = _CdiLib.gridDefGMEni3
gridDefLCC = _CdiLib.gridDefLCC
gridInqLCC = _CdiLib.gridInqLCC
gridDefLcc2 = _CdiLib.gridDefLcc2
gridInqLcc2 = _CdiLib.gridInqLcc2
gridDefLaea = _CdiLib.gridDefLaea
gridInqLaea = _CdiLib.gridInqLaea
gridDefArea = _CdiLib.gridDefArea
gridInqArea = _CdiLib.gridInqArea
gridHasArea = _CdiLib.gridHasArea
gridDefNvertex = _CdiLib.gridDefNvertex
gridInqNvertex = _CdiLib.gridInqNvertex
gridDefXbounds = _CdiLib.gridDefXbounds
gridInqXbounds = _CdiLib.gridInqXbounds
gridDefYbounds = _CdiLib.gridDefYbounds
gridInqYbounds = _CdiLib.gridInqYbounds
gridDefRowlon = _CdiLib.gridDefRowlon
gridInqRowlon = _CdiLib.gridInqRowlon
gridChangeType = _CdiLib.gridChangeType
zaxisName = _CdiLib.zaxisName
zaxisCreate = _CdiLib.zaxisCreate
zaxisDestroy = _CdiLib.zaxisDestroy
zaxisInqType = _CdiLib.zaxisInqType
zaxisInqSize = _CdiLib.zaxisInqSize
zaxisDuplicate = _CdiLib.zaxisDuplicate
zaxisResize = _CdiLib.zaxisResize
zaxisPrint = _CdiLib.zaxisPrint
zaxisSize = _CdiLib.zaxisSize
zaxisDefLevels = _CdiLib.zaxisDefLevels
zaxisInqLevels = _CdiLib.zaxisInqLevels
zaxisDefLevel = _CdiLib.zaxisDefLevel
zaxisInqLevel = _CdiLib.zaxisInqLevel
zaxisDefName = _CdiLib.zaxisDefName
zaxisDefLongname = _CdiLib.zaxisDefLongname
zaxisDefUnits = _CdiLib.zaxisDefUnits
zaxisInqName = _CdiLib.zaxisInqName
zaxisInqLongname = _CdiLib.zaxisInqLongname
zaxisInqUnits = _CdiLib.zaxisInqUnits
zaxisDefPrec = _CdiLib.zaxisDefPrec
zaxisInqPrec = _CdiLib.zaxisInqPrec
zaxisDefLtype = _CdiLib.zaxisDefLtype
zaxisInqLtype = _CdiLib.zaxisInqLtype
zaxisInqLevelsPtr = _CdiLib.zaxisInqLevelsPtr
zaxisDefVct = _CdiLib.zaxisDefVct
zaxisInqVctSize = _CdiLib.zaxisInqVctSize
zaxisInqVctPtr = _CdiLib.zaxisInqVctPtr
zaxisInqLbounds = _CdiLib.zaxisInqLbounds
zaxisInqUbounds = _CdiLib.zaxisInqUbounds
zaxisInqWeights = _CdiLib.zaxisInqWeights
zaxisInqLbound = _CdiLib.zaxisInqLbound
zaxisInqUbound = _CdiLib.zaxisInqUbound
zaxisDefLbounds = _CdiLib.zaxisDefLbounds
zaxisDefUbounds = _CdiLib.zaxisDefUbounds
zaxisDefWeights = _CdiLib.zaxisDefWeights
zaxisChangeType = _CdiLib.zaxisChangeType
taxisCreate = _CdiLib.taxisCreate
taxisDestroy = _CdiLib.taxisDestroy
taxisDuplicate = _CdiLib.taxisDuplicate
taxisCopyTimestep = _CdiLib.taxisCopyTimestep
taxisDefType = _CdiLib.taxisDefType
taxisDefVdate = _CdiLib.taxisDefVdate
taxisDefVtime = _CdiLib.taxisDefVtime
taxisDefRdate = _CdiLib.taxisDefRdate
taxisDefRtime = _CdiLib.taxisDefRtime
taxisHasBounds = _CdiLib.taxisHasBounds
taxisDefVdateBounds = _CdiLib.taxisDefVdateBounds
taxisDefVtimeBounds = _CdiLib.taxisDefVtimeBounds
taxisInqVdateBounds = _CdiLib.taxisInqVdateBounds
taxisInqVtimeBounds = _CdiLib.taxisInqVtimeBounds
taxisDefCalendar = _CdiLib.taxisDefCalendar
taxisDefTunit = _CdiLib.taxisDefTunit
taxisDefNumavg = _CdiLib.taxisDefNumavg
taxisInqType = _CdiLib.taxisInqType
taxisInqVdate = _CdiLib.taxisInqVdate
taxisInqVtime = _CdiLib.taxisInqVtime
taxisInqRdate = _CdiLib.taxisInqRdate
taxisInqRtime = _CdiLib.taxisInqRtime
taxisInqCalendar = _CdiLib.taxisInqCalendar
taxisInqTunit = _CdiLib.taxisInqTunit
taxisInqNumavg = _CdiLib.taxisInqNumavg
tunitNamePtr = _CdiLib.tunitNamePtr
institutDef = _CdiLib.institutDef
institutInq = _CdiLib.institutInq
institutInqNumber = _CdiLib.institutInqNumber
institutInqCenter = _CdiLib.institutInqCenter
institutInqSubcenter = _CdiLib.institutInqSubcenter
institutInqNamePtr = _CdiLib.institutInqNamePtr
institutInqLongnamePtr = _CdiLib.institutInqLongnamePtr
modelDef = _CdiLib.modelDef
modelInq = _CdiLib.modelInq
modelInqInstitut = _CdiLib.modelInqInstitut
modelInqGribID = _CdiLib.modelInqGribID
modelInqNamePtr = _CdiLib.modelInqNamePtr
tableWriteC = _CdiLib.tableWriteC
tableWrite = _CdiLib.tableWrite
tableRead = _CdiLib.tableRead
tableDef = _CdiLib.tableDef
tableInqNamePtr = _CdiLib.tableInqNamePtr
tableDefEntry = _CdiLib.tableDefEntry
tableInq = _CdiLib.tableInq
tableInqNumber = _CdiLib.tableInqNumber
tableInqNum = _CdiLib.tableInqNum
tableInqModel = _CdiLib.tableInqModel
tableInqPar = _CdiLib.tableInqPar
tableInqParCode = _CdiLib.tableInqParCode
tableInqParName = _CdiLib.tableInqParName
tableInqParLongname = _CdiLib.tableInqParLongname
tableInqParUnits = _CdiLib.tableInqParUnits
tableInqParNamePtr = _CdiLib.tableInqParNamePtr
tableInqParLongnamePtr = _CdiLib.tableInqParLongnamePtr
tableInqParUnitsPtr = _CdiLib.tableInqParUnitsPtr
streamDefHistory = _CdiLib.streamDefHistory
streamInqHistorySize = _CdiLib.streamInqHistorySize
streamInqHistoryString = _CdiLib.streamInqHistoryString


