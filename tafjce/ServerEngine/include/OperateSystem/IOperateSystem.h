#ifndef IOPERATESYSTEM_H_
#define IOPERATESYSTEM_H_

class IOperateSystem:
public IEntitySubsystem
{
public:
	virtual void gmGetPhoneSteam(string strPhoneNum) = 0;
	virtual void gmLogin() = 0;

	/**
	 * @brief 增加日充值活动的累积充值量
	 * <p>	充值模块需调用该接口	</p>
	 */
	virtual void IncDayPaymentPaied(int _dt) = 0;
};


#endif
