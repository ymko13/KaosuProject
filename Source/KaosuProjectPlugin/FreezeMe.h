#pragma once
class FreezeMe : public VAction
{
public:
  V_DECLARE_ACTION(FreezeMe)
  FreezeMe();
  ~FreezeMe();
  VOVERRIDE VBool Do(const class VArgList &argList);   
};

