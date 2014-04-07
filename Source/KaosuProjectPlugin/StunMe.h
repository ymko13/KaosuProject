#pragma once
class StunMe : public VAction
{
public:
  V_DECLARE_ACTION(StunMe)
  StunMe();
  ~StunMe();
  VOVERRIDE VBool Do(const class VArgList &argList);   
};
