#pragma once
class FearMe : public VAction
{
public:
  V_DECLARE_ACTION(FearMe)
  FearMe();
  ~FearMe();
  VOVERRIDE VBool Do(const class VArgList &argList);
};

