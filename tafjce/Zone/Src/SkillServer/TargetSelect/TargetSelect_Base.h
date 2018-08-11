#ifndef __TARGET_SELECT_BASE_H__
#define __TARGET_SELECT_BASE_H__

class SkillTargetSelect_Base
{
public:

	SkillTargetSelect_Base(){}
	virtual ~SkillTargetSelect_Base(){};

	virtual void selectTarget(HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList) = 0;

	// Common Function
	int getRow(int nPos)
	{
		int nRow = nPos/3;
		return nRow;
	}
	
	int getCol(int nPos)
	{
		int nCol = nPos % 3;
		return nCol;
	}
	
	int getPos(int nRow, int nCol)
	{
		return nRow * 3 + nCol;
	}

	bool isValid(HEntity hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		if(!pEntity) return false;

		int iHP = pEntity->getProperty(PROP_ENTITY_HP, 0);

		return iHP > 0;
	}

	// 屏蔽了神兽位置差异性,防止策划配置神兽单体技能导致问题
	int getFixBattlePos(HEntity hEntity)
	{
		IEntity* pEntity = getEntityFromHandle(hEntity);
		assert(pEntity);

		int iBattlePos = pEntity->getProperty(PROP_ENTITY_BATTLEPOS, 0);
		if(iBattlePos == GSProto::ATT_GOLDANIMAL_POS)
		{
			return 0;
		}
		else if(iBattlePos == GSProto::TARGET_GOLDANIMAL_POS)
		{
			return GSProto::MAX_BATTLE_MEMBER_SIZE/2;
		}

		return iBattlePos;
	}

	void selectAll(bool bSelf, HEntity hAttacker, vector<HEntity>& resultList, const vector<HEntity>& memberList)
	{
		IEntity* pEntity = getEntityFromHandle(hAttacker);
		assert(pEntity);

		int iSelfPos = getFixBattlePos(hAttacker);
		int iMinIndex = 0;
		int iMaxIndex = 0;
		if(bSelf)
		{
			if(iSelfPos < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
			{
				iMinIndex = 0;
				iMaxIndex = GSProto::MAX_BATTLE_MEMBER_SIZE/2;
			}
			else
			{
				iMinIndex = GSProto::MAX_BATTLE_MEMBER_SIZE/2;
				iMaxIndex = GSProto::MAX_BATTLE_MEMBER_SIZE;
			}
		}
		else
		{
			if(iSelfPos < GSProto::MAX_BATTLE_MEMBER_SIZE/2)
			{
				iMinIndex = GSProto::MAX_BATTLE_MEMBER_SIZE/2;
				iMaxIndex = GSProto::MAX_BATTLE_MEMBER_SIZE;
			}
			else
			{
				iMinIndex = 0;
				iMaxIndex = GSProto::MAX_BATTLE_MEMBER_SIZE/2;
			}
		}
		
		for(int i = iMinIndex; i < iMaxIndex; i++)
		{
			HEntity hTarget = memberList[i];
			IEntity* pTarget = getEntityFromHandle(hTarget);
			if(!pTarget)
			{
				continue;
			}

			int iHP = pTarget->getProperty(PROP_ENTITY_HP, (Int32)0);
			if(iHP <= 0)
			{
				continue;
			}
			
			resultList.push_back(hTarget);
		}
		
	}
};

#endif
