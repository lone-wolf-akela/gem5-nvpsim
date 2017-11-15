from m5.SimObject import SimObject
from BaseEnergySM import BaseEnergySM
from m5.params import *
from m5.proxy import *

class DFS_LRY(BaseEnergySM):
    type = 'DFS_LRY'
    cxx_header = "engy/DFS_LRY.hh"
    thres_off = Param.Float(Parent.thres_off, "threshold 'off' of energy state machine")
    thres_retention = Param.Float(Parent.thres_off, "threshold 'retention' of energy state machine")
    thres_2 = Param.Float(Parent.thres_off, "threshold '2' of energy state machine")
    thres_3 = Param.Float(Parent.thres_off, "threshold '3' of energy state machine")
    thres_4 = Param.Float(Parent.thres_off, "threshold '4' of energy state machine")
    thres_5 = Param.Float(Parent.thres_off, "threshold '5' of energy state machine")
