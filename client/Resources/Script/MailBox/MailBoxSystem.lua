--
--                       _oo0oo_
--                      o8888888o
--                      88" . "88
--                      (| O=O |)
--                      0\  ^  /0
--                    ___/`---'\___
--                  .' \\|     |-- '.
--                 / \\|||  :  |||-- \
--                / _||||| -:- |||||- \
--               |   | \\\  -  --/ |   |
--               | \_|  ''\---/''  |_/ |
--               \  .-\__  '-'  ___/-. /
--             ___'. .'  /--.--\  `. .'___
--          ."" '<  `.___\_<|>_/___.' >' "".
--         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
--         \  \ `_.   \_ __\ /__ _/   .-` /  /
--     =====`-.____`.___ \_____/___.-`___.-'=====
--                       `=---='
--
--
--     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
--
--               佛祖保佑         永无BUG
--

--[[" 
     邮箱系统
"]]


function FormatData(seconds)
   seconds = tonumber(seconds)
   local dataform = "%Y-%m-%d"
   return os.date(dataform, seconds)
end 

--邮箱列表list组件
function MailBoxSystem_MailList(widget,info)
   local maillist = UI_GetUIListView(widget, 3)
   maillist:removeAllItems()
   local templayout = Layout:create()
   templayout:setSize(CCSizeMake(UI_GetUILayout(widget, 4):getContentSize().width,UI_GetUILayout(widget, 4):getContentSize().height))
   maillist:setItemModel(templayout)
   local curlayout = nil 
   local function clickmaillayoutfunction(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then
         local sender = tolua.cast(sender,"Layout")
         local tag = maillist:getIndex(sender)
         Log("tag===="..tag)
         g_MailBoxSystem_ChooseIndex = tag 
         local tab = GameServer_pb.CMD_QUERY_MAILDESC_CS()
         tab.strUUID = g_MailBoxSystem_InfoList[tag + 1].strUUID
         Packet_Full(GameServer_pb.CMD_QUERY_MAILDESC, tab)
         ShowWaiting()
      end 
   end 
   local function createmail(index)
      --title 
      UI_SetLabelText(curlayout, 3, info.szMailIndexList[index].strMailTitle)

      --sender 
      UI_SetLabelText(curlayout, 4, info.szMailIndexList[index].strMailSender)

      --time
      UI_SetLabelText(curlayout, 5, FormatData(info.szMailIndexList[index].dwSendTime))
      
      --state
      if info.szMailIndexList[index].iState == GameServer_pb.en_MailState_UnRead then 
         --未读
         UI_GetUIImageView(curlayout, 2):loadTexture("MailBox/mailbox_maillist_letter_icon.png")
      elseif info.szMailIndexList[index].iState == GameServer_pb.en_MailState_Readed then 
         --正常已读
         UI_GetUIImageView(curlayout, 2):loadTexture("MailBox/mailbox_maillist_letter_open_icon.png")
      elseif info.szMailIndexList[index].iState == GameServer_pb.en_MailState_TakedAward then 
         --附件已领取
         UI_GetUIImageView(curlayout, 2):loadTexture("MailBox/mailbox_maillist_letter_open_icon.png")
      end 
   end 
   for i = 1,#info.szMailIndexList do 
      curlayout = UI_GetCloneLayout(UI_GetUILayout(widget, 4))
      curlayout:setVisible(true)
      maillist:pushBackCustomItem(curlayout)
      createmail(i)
      curlayout:setTouchEnabled(true)
      curlayout:addTouchEventListener(clickmaillayoutfunction)
   end 
end 

--信件详细信息
function MailBoxSystem_ShowCurMailInfo(info)
   local widget = UI_CreateBaseWidgetByFileName("MailBoxCurInfo.json",EUIOpenAction_Enlarge,0.25)
   local function clickReturnFunc(sender,eventType)
      if eventType == TOUCH_EVENT_ENDED then 
          UI_CloseCurBaseWidget(EUICloseAction_None)
          MailBoxSystem_GetMailBoxSystemListIndex(false)
          --ShowWaiting()
      end 
   end 
   UI_GetUILayout(widget, 1):addTouchEventListener(clickReturnFunc)
   local listview = UI_GetUIListView(widget, 5)
   local listviewwidth = listview:getContentSize().width 
   local listviewheight = listview:getContentSize().height 
   local layout = Layout:create()
   --title
   local titlelab = Label:create()
   UI_SetLabelFontName(1,titlelab)
   titlelab:setText(info.mailData.strMailTitle)
   titlelab:setFontSize(30)
   local titlewidth = titlelab:getContentSize().width
   local titleheight = titlelab:getContentSize().height
   layout:addChild(titlelab,100,100)
   titlelab:setPosition(ccp(listviewwidth / 2,listviewheight - titleheight / 2))
   titlelab:setColor(ccc3(255,255,0))
   local offsetY = 10
   local imagelayout = UI_GetCloneLayout(UI_GetUILayout(widget,7))
   imagelayout:setVisible(true)
   layout:addChild(imagelayout)
   imagelayout:setPosition(ccp(0,listviewheight - titleheight))

   --info 
   local infolab = CompLabel:GetDefaultCompLabel(FormatString("MailBoxSystem_test"),listviewwidth)
   local infolab = CompLabel:GetDefaultCompLabel(FormatString("MailBoxSystem_Info",info.mailData.strContent),listviewwidth)
   infolab:setAnchorPoint(ccp(0,1))
   local infoheight = infolab:getContentSize().height 
   infolab:setPosition(ccp(0,listviewheight - titleheight - offsetY))
   layout:addChild(infolab)
   --local infolab = CCLabelTTF:create(info.mailData.strContent,"Arial",24)
   --listview:addNode(infolab)
   --local infoheight = infolab:getContentSize().height 
   --local totalHeight = infoheight + titlewidth
   --infolab:setAnchorPoint(ccp(0,1))
   --infolab:setPosition(ccp(0,listviewheight - titleheight - offsetY))
   --infolab:setDimensions(CCSize(listviewwidth,0))
   --infolab:setColor(ccc3(127,255,0))

   
   
   --sender 
   local senderlab = Label:create()
   UI_SetLabelFontName(1,senderlab)
   senderlab:setText(info.mailData.strMailSender)
   senderlab:setFontSize(24)
   senderlab:setAnchorPoint(ccp(1,1))
   senderlab:setColor(ccc3(255,0,0))
   senderlab:setPosition(ccp(listviewwidth - 10,listviewheight - titleheight - infoheight - offsetY * 2))
   layout:addChild(senderlab)
   local positionheight =  titleheight + infoheight + offsetY * 2
   if info.mailData:HasField("awardInfo") then 
      --有奖励
      --奖励
      --隔条
      local getiaolayout = UI_GetCloneLayout(UI_GetUILayout(widget,6))
      getiaolayout:setVisible(true)
      --listview:pushBackCustomItem(getiaolayout)
      getiaolayout:setPosition(ccp(0,listviewheight - positionheight - offsetY * 3 - 10))
      layout:addChild(getiaolayout)

      g_MailBoxSystem_HaveReward = true 
      local rewardtitlelab = Label:create()
      UI_SetLabelFontName(1,rewardtitlelab)
      rewardtitlelab:setText(FormatString("MailBoxSystem_Reward"))
      rewardtitlelab:setFontSize(24)
      rewardtitlelab:setColor(ccc3(127,255,0))
      positionheight = positionheight + rewardtitlelab:getContentSize().height
      --rewardtitlelab:setPosition(ccp(listviewwidth / 2,listviewheight - positionheight - offsetY * 3))
      rewardtitlelab:setPosition(ccp(rewardtitlelab:getContentSize().width / 2,listviewheight - positionheight - offsetY * 3))
      layout:addChild(rewardtitlelab)
      positionheight = positionheight + offsetY * 3 + rewardtitlelab:getContentSize().height
      positionheight = positionheight + offsetY * 2
      --reward 
     local rewardheight = 0 
     local rewardImagePosX = listviewwidth / 5
     local rewardnumberPosX = rewardImagePosX + 20
     for i = 1,#info.mailData.awardInfo.szAwardPropList do
         local lifeattid = info.mailData.awardInfo.szAwardPropList[i].iLifeAttID
         local lifeattvar = info.mailData.awardInfo.szAwardPropList[i].iValue
         local image = GetAttIconNoFrame(lifeattid)
         layout:addChild(image)
         image:setPosition(ccp(rewardImagePosX,listviewheight - positionheight - rewardheight))        
         local rewardlab = Label:create()
         UI_SetLabelFontName(1,rewardlab)
         rewardlab:setText("  X"..lifeattvar)--GetAttName(lifeattid).." : "..lifeattvar)
         layout:addChild(rewardlab)
         rewardlab:setAnchorPoint(ccp(0,0.5))
         rewardlab:setPosition(ccp(rewardnumberPosX , listviewheight - positionheight - rewardheight - 5))
         rewardlab:setFontSize(24)
         rewardlab:setColor(ccc3(127,255,0))
         rewardheight = rewardheight + image:getContentSize().height
     end 
     --item 
     for i = 1,#info.mailData.awardInfo.szAwardItemList do 
        local itemId = info.mailData.awardInfo.szAwardItemList[i].iItemID
        local number = info.mailData.awardInfo.szAwardItemList[i].iCount
        local Itemdata = GetGameData(DataFileItem,itemId,"stItemData")
        local imageName = "Icon/Item/"..Itemdata.m_icon..".png" 
        local itemimage = ImageView:create()
        itemimage:loadTexture(imageName)
        itemimage:setScale(0.7)
        itemimage:setPosition(ccp(rewardImagePosX,listviewheight - positionheight - rewardheight - 10))
        layout:addChild(itemimage)
       
        --number
        local numberlab = Label:create()
        UI_SetLabelFontName(1,numberlab)
        numberlab:setText("  X"..number)
        numberlab:setFontSize(24)
        numberlab:setColor(ccc3(127,255,0))
        numberlab:setAnchorPoint(ccp(0,0.5))
        numberlab:setPosition(ccp(rewardnumberPosX ,listviewheight - positionheight - rewardheight - 10))
        layout:addChild(numberlab)
        rewardheight = rewardheight + itemimage:getContentSize().height 
     end 

     --hero 
     for i = 1 , #info.mailData.awardInfo.szAwardHeroList do 
         --icon
         local heroId = info.mailData.awardInfo.szAwardHeroList[i].iHeroID
         local heroLvelStep = info.mailData.awardInfo.szAwardHeroList[i].iLevelStep
         local number = info.mailData.awardInfo.szAwardHeroList[i].iCount
         local herodate = GetGameData(DataFileHeroBorn,heroId,"stHeroBornData")
         local heroimage = ImageView:create() 
         heroimage:loadTexture("Icon/HeroIcon/"..herodate.m_icon..".png")
         heroimage:setScale(0.5)
         heroimage:setPosition(ccp(rewardImagePosX,listviewheight - positionheight - rewardheight - 10))
         layout:addChild(heroimage)
         --frame 
         local frameimage = ImageView:create()
         local pingzi = GetHeroPingzi(heroLvelStep)
         frameimage:loadTexture("Common/Icon_Bg_00"..pingzi..".png")
         frameimage:setScale(0.5)
         frameimage:setPosition(ccp(rewardImagePosX,listviewheight - positionheight - rewardheight - 10))
         layout:addChild(frameimage)
         
         --count 
         --number
         local numberlab = Label:create()
         UI_SetLabelFontName(1,numberlab)
         numberlab:setText("  X"..number)
         numberlab:setFontSize(24)
         numberlab:setColor(ccc3(127,255,0))
         numberlab:setAnchorPoint(ccp(0,0.5))
         numberlab:setPosition(ccp(rewardnumberPosX ,listviewheight - positionheight - rewardheight - 10))
         layout:addChild(numberlab)
         positionheight = positionheight + frameimage:getContentSize().height * 0.5
     end
     --button get 
     local function MailBoxSystem_BeGet(sender,eventType)
        if eventType == TOUCH_EVENT_ENDED then 
           local tab = GameServer_pb.CMD_GET_MAILAWARD_CS()
           tab.strUUID = info.mailData.strUUID
           Packet_Full(GameServer_pb.CMD_GET_MAILAWARD, tab)
           ShowWaiting()
        end 
     end 
     UI_GetUIButton(widget, 2):setTitleText(FormatString("MailBoxSystem_Get"))
     UI_GetUIButton(widget, 2):setTitleFontSize(24)
     UI_GetUIButton(widget, 2):addTouchEventListener(MailBoxSystem_BeGet)
   else 
      --木有奖励
      --button 
      g_MailBoxSystem_HaveReward = false
      local function MailBoxSystem_BeSure(sender,eventType)
         if eventType == TOUCH_EVENT_ENDED then 
            UI_CloseCurBaseWidget(EUICloseAction_None)
            MailBoxSystem_GetMailBoxSystemListIndex(false)
         end 
      end 
      UI_GetUIButton(widget, 2):setTitleText(FormatString("MailBoxSystem_BeSure"))
      UI_GetUIButton(widget, 2):setTitleFontSize(24)
      UI_GetUIButton(widget, 2):addTouchEventListener(MailBoxSystem_BeSure)
   end 
   Log("positionheight===="..positionheight)
   layout:setSize(CCSizeMake(listviewwidth,listview:getContentSize().height))
   local templayout1 = Layout:create()
   templayout1:setSize(CCSizeMake(listviewwidth,positionheight + 120))
   layout:setPosition(ccp(0,(positionheight + 120 - listview:getContentSize().height)))
   templayout1:addChild(layout)
   --listview:insertCustomItem(layout,0)
   listview:pushBackCustomItem(templayout1)
end 