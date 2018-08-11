
function PushMentSystem_GetStr(src)
   local tab = {}
   local function spilit(txt)
      table.insert(tab,txt)
   end
   string.gsub(src,"(%w+)#?",spilit)
   return tab
end

function PushMentSystem_AZDelayTime(time)
   local timetable = PushMentSystem_GetStr(time)
   local hour = TypeConvert:ToInt(timetable[1])
   local min = TypeConvert:ToInt(timetable[2])
   local delaytime = TBTSDK:GetInstance():getTimer_AZ(hour,min)
   Log("delaytime")  
   return delaytime
end

function PushMentSystem_Push_AZ()
   local pushmentdata = CDataManager:GetInstance():GetGameDataKeyList(DataFilePushMent)
   local pushmentsize = pushmentdata:size()
   Log("pushmentsize===="..pushmentsize)
   for i = 1,pushmentsize do 
      local data = GetGameData(DataFilePushMent, i, "stPushMentData")
      local delaytime = PushMentSystem_AZDelayTime(data.m_time)
      TBTSDK:GetInstance():pushMent(data.m_info.."#"..data.m_Title,delaytime,i,"")
   end 
end