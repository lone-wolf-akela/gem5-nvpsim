#include "DFS_LRY.hh"
#include "debug/EnergyMgmt.hh"

DFS_LRY::DFS_LRY(const Params *p)
    : BaseEnergySM(p), 
    	state(DFS_LRY::State::STATE_INIT),
      thres_off(p->thres_off), 
      thres_2(p->thres_2),
      thres_3(p->thres_3),
      thres_4(p->thres_4),
      thres_5(p->thres_5)
{

}

void DFS_LRY::init()
{
    state = DFS_LRY::State::STATE_POWEROFF;
    EnergyMsg msg;
    msg.val = 0;
    msg.type = MsgType::POWEROFF;
    broadcastMsg(msg);
}

void DFS_LRY::update(double _energy)
{
    EnergyMsg msg;
    msg.val = 0;

//Initialization
    if (state == STATE_INIT)
    {
        state = STATE_POWEROFF;
    }

//change from higher freq to lower freq
    else if (state == STATE_FREQ5 && _energy < thres_5)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ5->STATE_FREQ4 state=%d, _energy=%lf, thres_5=%lf\n", state, _energy, thres_5);
        state = STATE_FREQ4;
        msg.type = MsgType::FREQ5to4;
        broadcastMsg(msg);
    }

    else if (state == STATE_FREQ4 && _energy < thres_4)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ4->STATE_FREQ3 state=%d, _energy=%lf, thres_4=%lf\n", state, _energy, thres_4);
        state = STATE_FREQ3;
        msg.type = MsgType::FREQ4to3;
        broadcastMsg(msg);
    }
    
    else if (state == STATE_FREQ3 && _energy < thres_3)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ3->STATE_FREQ2 state=%d, _energy=%lf, thres_3=%lf\n", state, _energy, thres_3);
        state = STATE_FREQ2;
        msg.type = MsgType::FREQ3to2;
        broadcastMsg(msg);
    }
    
    else if (state == STATE_FREQ2 && _energy < thres_2)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ2->STATE_FREQ1 state=%d, _energy=%lf, thres_2=%lf\n", state, _energy, thres_2);
        state = STATE_FREQ1;
        msg.type = MsgType::FREQ2to1;
        broadcastMsg(msg);
    }
    
//change from lower freq to higher freq
    else if (state == STATE_FREQ4 && _energy > thres_5)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ4->STATE_FREQ5 state=%d, _energy=%lf, thres_5=%lf\n", state, _energy, thres_5);
        state = STATE_FREQ5;
        msg.type = MsgType::FREQ4to5;
        broadcastMsg(msg);
    }

    else if (state == STATE_FREQ3 && _energy > thres_4)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ3->STATE_FREQ4 state=%d, _energy=%lf, thres_4=%lf\n", state, _energy, thres_4);
        state = STATE_FREQ4;
        msg.type = MsgType::FREQ3to4;
        broadcastMsg(msg);
    }
    
    else if (state == STATE_FREQ2 && _energy > thres_3)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ2->STATE_FREQ3 state=%d, _energy=%lf, thres_3=%lf\n", state, _energy, thres_3);
        state = STATE_FREQ3;
        msg.type = MsgType::FREQ2to3;
        broadcastMsg(msg);
    }
    
    else if (state == STATE_FREQ1 && _energy > thres_2)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ1->STATE_FREQ2 state=%d, _energy=%lf, thres_2=%lf\n", state, _energy, thres_2);
        state = STATE_FREQ2;
        msg.type = MsgType::FREQ1to2;
        broadcastMsg(msg);
    }

//power on 
    else if (state == STATE_POWEROFF && _energy > thres_5)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_POWEROFF->STATE_FREQ5 state=%d, _energy=%lf, thres_5=%lf\n", state, _energy, thres_5);
        state = STATE_FREQ5;
        msg.type = MsgType::POWERON;
        broadcastMsg(msg);
    }

//pwoer off
    else if (state == STATE_FREQ1 && _energy < thres_off)
    {
        DPRINTF(EnergyMgmt, "State change: STATE_FREQ1->STATE_POWEROFF state=%d, _energy=%lf, thres_off=%lf\n", state, _energy, thres_off);
        state = STATE_POWEROFF;
        msg.type = MsgType::POWEROFF;
        broadcastMsg(msg);
    }
}

DFS_LRY *
DFS_LRYParams::create()
{
    return new DFS_LRY(this);
}
