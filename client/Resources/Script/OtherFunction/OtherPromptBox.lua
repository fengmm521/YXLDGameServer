-- 上浮提示框
l_promptBoxtable = {}
l_promptBoxDelayTime = {}
l_promptActiontable = {}
l_promptBoxlabeltable = {}
l_promptBoxlayout = nil 
--初始化数据
function OtherPromptBox_Init()
   l_promptBoxtable = {}
   l_promptBoxDelayTime = {}
   l_promptActiontable = {}
   l_promptBoxlabeltable = {}
   l_promptBoxlayout = nil 
end 

function OtherPromptBox_InitValue(string)
   table.insert(l_promptBoxtable,string)
end

function OtherPromptBox_StarPromptAction(pos)
   l_promptBoxlayout = Layout:create()
   OtherPromptBox_InitPromptActionValue()
   OtherPromptBox_Initlabel(pos)
   MainScene:GetInstance():AddChild(l_promptBoxlayout, EMSTag_OtherPromptBox, EMSTag_OtherPromptBox, false) 
   l_promptBoxlayout:setPosition(ccp(480,320))
   for i = 1 , #l_promptActiontable do 
      l_promptBoxlabeltable[i]:runAction(l_promptActiontable[i])
   end 
end  

function OtherPromptBox_removeAction(sender)
   sender:removeFromParent()
end 

function OtherPromptBox_removeChild()
   MainScene:GetInstance():removeChildByTag(EMSTag_OtherPromptBox)
end 

function OtherPromptBox_Visible(sender)
   sender:setVisible(true)
end 

function OtherPromptBox_InitPromptActionValue()
   local baseofftime = 0.6
   for i = 1, #l_promptBoxtable do 
      table.insert(l_promptBoxDelayTime,baseofftime * (i - 1))
   end 
   local movetime = 0.25
   local basepos = 24
   for i = 1,#l_promptBoxDelayTime do 
      local actionArry = CCArray:create()
      local delaytime = CCDelayTime:create(l_promptBoxDelayTime[i])
      actionArry:addObject(delaytime)
      local callfuncone = CCCallFuncN:create(OtherPromptBox_Visible)
      actionArry:addObject(callfuncone)

      local bigaction = CCScaleTo:create(movetime,2)
      local normoralaction = CCScaleTo:create(movetime,1)
      actionArry:addObject(bigaction)
      actionArry:addObject(normoralaction)

      local actionone = CCMoveBy:create(movetime * 3,ccp(0,basepos * 3))
      actionArry:addObject(actionone)

      local callfunc = CCCallFuncN:create(OtherPromptBox_removeAction)
      actionArry:addObject(callfunc)
      local seq = CCSequence:create(actionArry)
      if i == #l_promptBoxDelayTime then 
         local callfuncremove = CCCallFuncN:create(OtherPromptBox_removeChild)
         actionArry:addObject(callfuncremove)
      end 
      table.insert(l_promptActiontable,seq)
   end  
end 

function OtherPromptBox_Initlabel(pos)
   for i = 1,#l_promptBoxtable do 
      local lab = Label:create()
      lab:setText(l_promptBoxtable[i])
      lab:setFontSize(24)
      lab:setColor(ccc3(0,255,0))
      lab:setVisible(false)
      lab:setAnchorPoint(ccp(0.5,0.5))
      UI_SetLabelFontName(1,lab)
      lab:enableStroke()
      if pos ~= nil then 
         lab:setPosition(ccp(pos.x - 480,pos.y - 320))
      end 
      table.insert(l_promptBoxlabeltable,lab)
      l_promptBoxlayout:addChild(lab)
   end 
end 