#pragma once
class PoisonMe : public VAction
{
public:
  V_DECLARE_ACTION(PoisonMe)
  PoisonMe();
  ~PoisonMe();
  VOVERRIDE VBool Do(const class VArgList &argList);   
};

