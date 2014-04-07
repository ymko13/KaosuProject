#pragma once
class BurnMe : public VAction
{
public:
  V_DECLARE_ACTION(BurnMe)
  BurnMe();
  ~BurnMe();
  VOVERRIDE VBool Do(const class VArgList &argList);   
};

