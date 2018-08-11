--                            _ooOoo_  
--                           o8888888o  
--                           88" . "88  
--                           (| -_- |)  
--                            O\ = /O  
--                        ____/`---'\____  
--                      .   ' \\| |// `.  
--                       / \\||| : |||// \  
--                     / _||||| -:- |||||- \  
--                       | | \\\ - /// | |  
--                     | \_| ''\---/'' | |  
--                      \ .-\__ `-` ___/-. /  
--                   ___`. .' /--.--\ `. . __  
--                ."" '< `.___\_<|>_/___.' >'"".  
--               | | : `- \`.;`\ _ /`;.`/ - ` : | |  
--                 \ \ `-. \_ __\ /__ _/ .-` / /  
--         ======`-.____`-.___\_____/___.-`____.-'======  
--                            `=---='  
--  
--         .............................................  
--                  佛祖镇楼                  BUG辟易  

--[[" 
      新英雄装备 Begin
"]]

--[[" 
      1 4    武器 戒指
      2 5    衣服 奇物
      3 6    鞋子 秘籍
"]]

function NewHeroEquip_InitValue()

end

function NewHeroEquip_ShowLayout()
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   if widget == nil then 
      return 
   end 
   local layout = UI_GetUILayout(widget,8)
   if layout:isVisible() == false then 
       g_heroCurInfoLayoutState = 5 
       HeroCurInfoSecond_ShowCurLayout(widget,slectheroEntity) 
   else 
      
   end  
   local heroEntity = HeroListSecond_GetHeroEntityByIndex()
    NewHeroEquip_InitValue()
    local tag = {1,2,3,4,5,6}

    NewHeroEquip_InitLayoutEquip(layout,heroEntity)
    local function HeroEquip_Click(templayout,index)
        local function clicklayoutFunc(sender,eventType)
            if eventType == TOUCH_EVENT_ENDED then 
				local isWear = heroEntity:GetEquipBags():HeroEquipIsWear(index - 1)
				NewHeroEquip_ClickIndex(layout,heroEntity,index - 1,isWear)
				if (g_isGuide) then
					Guide_GoNext();
				end
            end  
        end
        templayout:setTouchEnabled(true)
        templayout:addTouchEventListener(clicklayoutFunc)
    end
    for i = 1,#tag do
        HeroEquip_Click(UI_GetUILayout(UI_GetUILayout(layout,2),tag[i]),i)
    end
    local function clickXilian(sender,eventType)
       if eventType == TOUCH_EVENT_ENDED then  
          createPromptBoxlayout(FormatString("EquipXilian_Limit"))
       end 
    end
    UI_GetUIButton(layout,1):addTouchEventListener(clickXilian)
end

function NewHeroEquip_RetrunEquipPos(equip)
   Log("EEquip_Pos==="..equip:GetInt(EEquip_Pos))
   return equip:GetInt(EEquip_Pos)
end


function NewHeroEquip_InitEquipTemp(tag)
   local str = ""
   if tag == 1 then 
      str = "zhuangbei/bg_zhuangbei_SY01.png"
   elseif tag == 2 then 
      str = "zhuangbei/bg_zhuangbei_SY03.png"
   elseif tag == 3 then 
      str = "zhuangbei/bg_zhuangbei_SY05.png"
   elseif tag == 4 then 
      str = "zhuangbei/bg_zhuangbei_SY02.png"
   elseif tag == 5 then 
      str = "zhuangbei/bg_zhuangbei_SY04.png"
   elseif tag == 6 then 
      str = "zhuangbei/bg_zhuangbei_SY06.png"
   end 
   return str 
end

function NewHeroEquip_InitLayoutEquip(layout,heroEntity)
   local tag = {1,2,3,4,5,6}
   --local wearList = heroEntity:GetEquipBags():GetEquipList()
   local heroEquipList = vector_Equip__:new_local()
   heroEntity:GetEquipBags():GetEquipList(heroEquipList)
   local imagelayout = UI_GetUILayout(layout,2)
--   for i = 1 , #tag do 
--      UI_GetUIImageView(UI_GetUIImageView(imagelayout,tag[i]),1):loadTexture("HeroSystem/Bg_003.png")
--   end 
   local notequipetag = {}
    for i=1 ,6 do
        UI_GetUIImageView(UI_GetUIImageView(imagelayout,i), 2):removeChildByTag(1025)
   end
   for i = 0,heroEquipList:size() - 1 do 
      --if tag[i + 1] == NewHeroEquip_RetrunEquipPos(heroEquipList[i]) + 1 then 
         local temptag = NewHeroEquip_RetrunEquipPos(heroEquipList[i]) + 1
         local equipdata = heroEquipList[i]:GetData()

         local icon = UI_ItemIconFrame(UI_GetEquipIcon(equipdata.m_icon), equipdata.m_quality)
         icon:setTag(1025)
         UI_GetUIImageView(UI_GetUIImageView(imagelayout,temptag), 2):addChild(icon)

         --UI_GetHeroEquipIcon(equipdata.m_icon,UI_GetUIImageView(imagelayout,temptag),1)
         --UI_IconFrameWithSkillOrItem(UI_GetUIImageView(imagelayout,temptag),2,equipdata.m_quality)
         table.insert(notequipetag,temptag)
      --end 
   end

   --判断是否有可穿戴的 
   local function isHave(number)
      local ishave = false 
      for i = 1,#notequipetag do
          if notequipetag[i] == number then 
             ishave = true
          end 
      end 
      return ishave
   end
   for i = 1,#tag do
      if isHave(tag[i]) == false then 
         Log("xxxxxxxxxx==="..i)
         local str = NewHeroEquip_InitEquipTemp(i)
         UI_GetUIImageView(UI_GetUIImageView(imagelayout,tag[i]),1):loadTexture(str)
         local herodata = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
         if GetLocalPlayer():GetEquipBags():HeroHasCanEquip(tag[i] - 1,herodata.m_heroType) then
            UI_GetUILabel(UI_GetUIImageView(imagelayout,tag[i]),3):setVisible(true)
            UI_GetUIImageView(UI_GetUIImageView(imagelayout,tag[i]),4):setVisible(true)
         else 
            UI_GetUILabel(UI_GetUIImageView(imagelayout,tag[i]),3):setVisible(false)
            UI_GetUIImageView(UI_GetUIImageView(imagelayout,tag[i]),4):setVisible(false)
         end
      else 
         Log("xxxxxxxxxxxxxxxxx==="..i)
         UI_GetUILabel(UI_GetUIImageView(imagelayout,tag[i]),3):setVisible(false)
         UI_GetUIImageView(UI_GetUIImageView(imagelayout,tag[i]),4):setVisible(false)
      end 
   end
end


function NewHeroEquip_ClickIndex(layout,heroEntity,index,isHaveWear)
   Log("index===="..index)
   if isHaveWear then 
      NewHeroEquip_ChooseEquip_InfoLayout(heroEntity,index)
   else 
      NewHeroEquip_ChooseEquip_Layout(heroEntity,index)
   end 
end


function NewHeroEquip_QuerHeroEquip(heroEntity)
   local tab = GameServer_pb.CMD_QUERY_HEROEQUIP_CS()
   tab.dwHeroObjectID = heroEntity:GetUint(EHero_ID)
   Packet_Full(GameServer_pb.CMD_QUERY_HEROEQUIP, tab)
end

function NewHeroEquip_ChooseEquip_Layout(heroEntity,index)
   local widget = UI_CreateBaseWidgetByFileName("HeroSystemEquipmentTwo.json",EUIOpenAction_FadeIn,0.5)
   NewHeroEquip_ChooseEquip_Choose(widget,heroEntity,index)
   
end

function NewHeroEquip_ChooseEquip_Choose(widget,heroEntity,index)
   local bagList = vector_Equip__:new_local()
   local herodata = GetGameData(DataFileHeroBorn,heroEntity:GetUint(EHero_HeroId),"stHeroBornData")
   GetLocalPlayer():GetEquipBags():getSizeUpEquip(bagList,index,herodata.m_heroType)
   local listview = UI_GetUIListView(widget,1)
   listview:removeAllItems()
   local listwidth = listview:getContentSize().width
   local listheight = listview:getContentSize().height 

   local layout = nil 

   local function createTemplayout(layout,index)
      local equipdata = bagList[index]:GetData()
      --name
      UI_SetLabelText(layout,3,equipdata.m_name)
      --lvl
      UI_SetLabelText(UI_GetUIImageView(layout,8),1,equipdata.m_lvlLimit)
      --info
      UI_SetLabelText(layout,4,equipdata.m_attinfo)
      --otherinfo
        local icon = UI_ItemIconFrame(UI_GetEquipIcon(equipdata.m_icon), equipdata.m_quality)
        UI_GetUIImageView(layout,2):addChild(icon)
      --icon
      --UI_GetHeroEquipIcon(equipdata.m_icon,layout,1)
      ----frame
      --UI_IconFrameWithSkillOrItem(layout,2,equipdata.m_quality)

      --addabstri 
      local addabstri = bagList[index]:getAddAttribute()
      if addabstri.lifeattId == 0 then 
         UI_GetUILabel(layout,5):setVisible(false)
      else 
         UI_GetUILabel(layout,5):setVisible(true)
         local str = GetAttNameAndValue(addabstri.lifeattId,addabstri.lifeattValue)
         UI_SetLabelText(layout,5,str)
      end 
      --button
          local function ClickWear(sender,eventType)
              if eventType == TOUCH_EVENT_ENDED then 
                 Equip_Wear(bagList[index],heroEntity)
				 if (g_isGuide) then
					Guide_GoNext();
				 end
              end 
          end
          UI_GetUIButton(layout,6):addTouchEventListener(ClickWear)
   end
   --第一个是装备的
   local equipHave = heroEntity:GetEquipBags():GetEquipByPos(index)
   local function createTemplayoutWithhave(layout,equipHave)
      --UI_GetUIButton(layout,6):setVisible(false)
      --UI_GetUILabel(layout,100):setVisible(true)
      local equipdata = equipHave:GetData()
      --name
      UI_SetLabelText(layout,3,equipdata.m_name)
      --lvl
      UI_SetLabelText(UI_GetUIImageView(layout,8),1,equipdata.m_lvlLimit)
      --info
      UI_SetLabelText(layout,4,equipdata.m_attinfo)
      --otherinfo

      --icon
        local icon = UI_ItemIconFrame(UI_GetEquipIcon(equipdata.m_icon), equipdata.m_quality)
        UI_GetUIImageView(layout,2):addChild(icon)
      --UI_GetHeroEquipIcon(equipdata.m_icon,layout,1)
      ----frame
      --UI_IconFrameWithSkillOrItem(layout,2,equipdata.m_quality)

      --addabstri 
      local addabstri = equipHave:getAddAttribute()
      if addabstri.lifeattId == 0 then 
         UI_GetUILabel(layout,5):setVisible(false)
      else 
         UI_GetUILabel(layout,5):setVisible(true)
         local str = GetAttNameAndValue(addabstri.lifeattId,addabstri.lifeattValue)
         UI_SetLabelText(layout,5,str)
      end 
      UI_GetUIButton(layout,6):setVisible(false)
      UI_GetUIImageView(layout,25):setVisible(true)
   end
   if equipHave ~= nil then 
      local templayout = UI_GetCloneLayout(UI_GetUILayout(widget,2))
      templayout:setVisible(true)
      createTemplayoutWithhave(templayout,equipHave)
      listview:pushBackCustomItem(templayout)
   end  

   for i = 0,bagList:size() - 1 do 
      layout = UI_GetCloneLayout(UI_GetUILayout(widget,2))
      layout:setVisible(true)
      createTemplayout(layout,i)
      listview:pushBackCustomItem(layout)
   end  
   UI_GetUILayout(widget,100):setVisible(false)
   if bagList:size() == 0 then 
      if equipHave == nil then
         NewHeroEquip_GoFuben(widget,index)
      else
         NewHeroEquip_GoFuben(widget,index,false)
      end 
      
   end 
end

function NewHeroEquip_GoFuben(widget,index,ishave)
   local layout = UI_GetUILayout(widget,100)
   layout:setVisible(true)
   if ishave ~= nil and ishave == false then
      UI_GetUILabel(layout,1124848802):setVisible(false)
   end
   Log("index===="..index)
   local iconstring = "zhuangbeiFB/btn_zhuangbeiFB_0"..(index + 1)..".png"
   local function clicklayoutFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseAllBaseWidget()
         g_EquipFuben_SelectIndex = g_EquipFubenTable[index + 1]
         local tab = GameServer_pb.Cmd_Cs_QueryDSectionDetail()
         tab.iSectionID = g_EquipFubenTable[index + 1]
         Packet_Full(GameServer_pb.CMD_QUERY_DSECTIONDETAIL, tab)
         ShowWaiting()
      end  
   end
   UI_GetUIImageView(layout,1):loadTexture(iconstring)
   UI_GetUIImageView(layout,1):setTouchEnabled(true)
   UI_GetUIImageView(layout,1):addTouchEventListener(clicklayoutFunc)
end


function NewHeroEquip_ChooseEquip_InfoLayout(heroEntity,index)
   local widget = UI_CreateBaseWidgetByFileName("HeroSystemEquipmentOne.json",EUIOpenAction_FadeIn,0.5)
   NewHeroEquip_ChooseEquip_Info(widget,heroEntity,index)
end

function NewHeroEquip_ChooseEquip_Info(widget,heroEntity,index)
   --local heroEquipList = vector_Equip__:new_local()
   --heroEntity:GetEquipBags():GetEquipList(heroEquipList)
   local heroEquipList = heroEntity:GetEquipBags():GetEquipList()
   if heroEquipList[index] == nil then 
      return 
   end 
   local equipdata = heroEquipList[index]:GetData()
   --name
   UI_SetLabelText(widget,2,equipdata.m_name)
   --icon
       local icon = UI_ItemIconFrame(UI_GetEquipIcon(equipdata.m_icon), equipdata.m_quality)
        UI_GetUIImageView(UI_GetUIImageView(widget,1),3):addChild(icon)
   --UI_GetHeroEquipIcon(equipdata.m_icon,UI_GetUIImageView(widget,1),2)
   ----frame
   --UI_IconFrameWithSkillOrItem(UI_GetUIImageView(widget,1),3,equipdata.m_quality)
   --职业
   UI_SetLabelText(widget,3,FormatString("BagLayer_limit1",FormatString("BagLayer_limit_"..equipdata.m_zhiyeLimit)))
   --职业
   UI_SetLabelText(widget,1124813845,FormatString("BagLayer_limit2",equipdata.m_lvlLimit))

   --info
   local infolayout = UI_GetUILayout(widget,4)
   local layoutwidth = infolayout:getContentSize().width
   local layoutheight = infolayout:getContentSize().height
   local infostrlab = Label:create()
   infostrlab:setText(FormatString("HeroEquip_BaseInfo").."\n".."  "..equipdata.m_attinfo)
   UI_SetLabelFontName(1,infostrlab)
   infostrlab:setFontSize(20)
   infolayout:addChild(infostrlab)
   infostrlab:setAnchorPoint(ccp(0,1))
   infostrlab:setPosition(ccp(0,layoutheight))
   local infostrlabHeight = infostrlab:getContentSize().height
   local addabstri = heroEquipList[index]:getAddAttribute()
   if addabstri.lifeattId ~= 0 then 
      local addabstristr = FormatString("HeroEquip_AddInfo").."\n".."  "..GetAttNameAndValue(addabstri.lifeattId,addabstri.lifeattValue)
      local addabstrilab = Label:create()
      addabstrilab:setText(addabstristr)
      addabstrilab:setFontSize(20)
      addabstrilab:setAnchorPoint(ccp(0,1))
      addabstrilab:setPosition(ccp(0,layoutheight - infostrlabHeight - 10))
      addabstrilab:setColor(ccc3(167,44,176))
      infolayout:addChild(addabstrilab)
      UI_SetLabelFontName(1,addabstrilab)
   end 
--   --miaoshu 
--   local curinfolab = Label:create()
--   curinfolab:setText(equipdata.m_desc)
--   UI_SetLabelFontName(1,curinfolab)
--   curinfolab:setFontSize(20)
--   infolayout:addChild(curinfolab)
--   curinfolab:setAnchorPoint(ccp(0,0))
--   curinfolab:setPosition(ccp(10,0))
--   curinfolab:setColor(ccc3(250,128,10))

   --更换
   local function clickChangeFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         UI_CloseCurBaseWidget(EUICloseAction_None)
         NewHeroEquip_ChooseEquip_Layout(heroEntity,index)
      end 
   end
   UI_GetUIButton(widget,5):addTouchEventListener(clickChangeFunc)
    
   --卸下
   local function ClickRmoveFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
         Equip_Remove(heroEntity:GetEquipBags():GetEquipByPos(index),heroEntity)
      end 
   end
   UI_GetUIButton(widget,6):addTouchEventListener(ClickRmoveFunc)
end

--[[" 
      新英雄装备 End
"]]


function Equip_Wear(equip,heroEntity)
	local tab = GameServer_pb.CMD_EQUIP_MOVE_CS();

	tab.dwSrcObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);
	tab.dwDstObjectID = heroEntity:GetUint(EHero_ID);	
	
	tab.iSrcPos = equip:GetInt(EEquip_Pos);	
	tab.iDstPos = -1;	
	Packet_Full(GameServer_pb.CMD_EQUIP_MOVE, tab);
	ShowWaiting();
end

function Equip_Remove(equip,heroEntity)
   local tab = GameServer_pb.CMD_EQUIP_MOVE_CS();

   tab.dwSrcObjectID = heroEntity:GetUint(EHero_ID);
   tab.dwDstObjectID = GetLocalPlayer():GetInt(EPlayer_ObjId);	
	
   tab.iSrcPos = equip:GetInt(EEquip_Pos);	
   tab.iDstPos = -1;	
   Packet_Full(GameServer_pb.CMD_EQUIP_MOVE, tab);
   ShowWaiting();
end

function NewHeroEquip_AfterEquip()
   EndWaiting()
   UI_CloseCurBaseWidget(EUICloseAction_None)
   local widget = UI_GetBaseWidgetByName("HeroSystemSecond")
   if widget == nil then 
      return 
   end 
   local heroEntity = HeroListSecond_GetHeroEntityByIndex()
   NewHeroEquip_InitValue()
   local layout = UI_GetUILayout(widget,8)
   local tag = {1,2,3,4,5,6}

   NewHeroEquip_InitLayoutEquip(layout,heroEntity)
   local function HeroEquip_Click(templayout,index)
      local function clicklayoutFunc(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            local isWear = heroEntity:GetEquipBags():HeroEquipIsWear(index - 1)
            NewHeroEquip_ClickIndex(layout,heroEntity,index - 1,isWear)
         end  
      end
      templayout:setTouchEnabled(true)
      templayout:addTouchEventListener(clicklayoutFunc)
   end
   for i = 1,#tag do
       HeroEquip_Click(UI_GetUILayout(UI_GetUILayout(layout,2),tag[i]),i)
   end
   HeroCurInfoSecond_HeroBaseInfoShow(widget,heroEntity)  
end

function NewHeroEquip_AfterEquip_RefreshBag()

end