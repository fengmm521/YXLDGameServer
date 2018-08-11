@echo off

echo ...Export Numeric for Client...
echo ...Hero...
%DATAEXPORTER% -src %CD%\Hero\Hero.csv -dataName HeroBornData -dataDes %CD%\Hero\Hero_Description_Client.xml -output %CD%\export\HeroBornData.bin
%DATAEXPORTER% -src %CD%\Hero\HeroCreate.csv -dataName HeroCreateData -dataDes %CD%\Hero\HeroCreate_Description_Client.xml -output %CD%\export\HeroCreateData.bin
%DATAEXPORTER% -src %CD%\Hero\HeroTallent.csv -dataName HeroTallentBornData -dataDes %CD%\Hero\HeroTallent_Description_Client.xml -output %CD%\export\HeroTallentBornData.bin
%DATAEXPORTER% -src %CD%\Hero\LevelStepGrow.csv -dataName LevelStepGrowData -dataDes %CD%\Hero\LevelStepGrow_Description_Client.xml -output %CD%\export\LevelStepGrowData.bin
%DATAEXPORTER% -src %CD%\Hero\HeroLevelExp.csv -dataName HeroLevelExpData -dataDes %CD%\Hero\HeroLevelExp_Description_Client.xml -output %CD%\export\HeroLevelExpData.bin
%DATAEXPORTER% -src %CD%\Hero\HeroQuality.csv -dataName HeroQualityData -dataDes %CD%\Hero\HeroQuality_Description_Client.xml -output %CD%\export\HeroQualityData.bin
%DATAEXPORTER% -src %CD%\Hero\QualityProp.csv -dataName QualityPropData -dataDes %CD%\Hero\QualityProp_Description_Client.xml -output %CD%\export\QualityPropData.bin

echo ...Monster...
%DATAEXPORTER% -src %CD%\Monster\Monster.csv -dataName MonsterData -dataDes %CD%\Monster\Monster_Description_Client.xml -output %CD%\export\MonsterData.bin

echo ...String...
%DATAEXPORTER% -src %CD%\String\CreateName.csv -dataName CreateNameData -dataDes %CD%\String\CreateName_Description_Client.xml -output %CD%\export\CreateNameData.bin

echo ...Scene...
%DATAEXPORTER% -src %CD%\Scene\Scene.csv -dataName SceneData -dataDes %CD%\Scene\Scene_Description_Client.xml -output %CD%\export\SceneData.bin
%DATAEXPORTER% -src %CD%\Scene\SceneSection.csv -dataName SceneSectionData -dataDes %CD%\Scene\SceneSection_Description_Client.xml -output %CD%\export\SceneSectionData.bin
%DATAEXPORTER% -src %CD%\Scene\PerfectPassAward.csv -dataName PerfectPassAwardData -dataDes %CD%\Scene\PerfectPassAward_Description_Client.xml -output %CD%\export\PerfectPassAwardData.bin

echo ...Skill...
%DATAEXPORTER% -src %CD%\Skill\Skill.csv -dataName SkillData -dataDes %CD%\Skill\Skill_Description_Client.xml -output %CD%\export\SkillData.bin
%DATAEXPORTER% -src %CD%\Skill\SkillClientEffect.csv -dataName SkillEffectData -dataDes %CD%\Skill\SkillClientEffect_Description_Client.xml -output %CD%\export\SkillEffectData.bin
%DATAEXPORTER% -src %CD%\Skill\Buff.csv -dataName BuffData -dataDes %CD%\Skill\Buff_Description_Client.xml -output %CD%\export\BuffData.bin
%DATAEXPORTER% -src %CD%\Skill\ActorSkill.csv -dataName ActorSkillData -dataDes %CD%\Skill\ActorSkill_Description_Client.xml -output %CD%\export\ActorSkillData.bin

echo ...Item...
%DATAEXPORTER% -src %CD%\Item\Item.csv -dataName ItemData -dataDes %CD%\Item\Item_Description_Client.xml -output %CD%\export\ItemData.bin
%DATAEXPORTER% -src %CD%\Item\Equip.csv -dataName EquipData -dataDes %CD%\Item\Equip_Description_Client.xml -output %CD%\export\EquipData.bin
rem %DATAEXPORTER% -src %CD%\Item\EquipCombine.csv -dataName EquipCombineData -dataDes %CD%\Item\EquipCombine_Description_Client.xml -output %CD%\export\EquipCombineData.bin
%DATAEXPORTER% -src %CD%\Item\EquipSuit.csv -dataName EquipSuitData -dataDes %CD%\Item\EquipSuit_Description_Client.xml -output %CD%\export\EquipSuitData.bin

echo ......Favorite....
%DATAEXPORTER% -src %CD%\Item\FavoriteCombine.csv -dataName FavoriteData -dataDes %CD%\Item\Favorite_Description_Client.xml -output %CD%\export\FavoriteData.bin


echo ...FightSoul...
%DATAEXPORTER% -src %CD%\FightSoul\FightSoul.csv -dataName FightSoulData -dataDes %CD%\FightSoul\FightSoul_Description_Client.xml -output %CD%\export\FightSoulData.bin
%DATAEXPORTER% -src %CD%\FightSoul\FightSoulExchange.csv -dataName FightSoulExchangeData -dataDes %CD%\FightSoul\FightSoulExchange_Description_Client.xml -output %CD%\export\FightSoulExchangeData.bin

echo ...GodAnimal...
%DATAEXPORTER% -src %CD%\GodAnimal\GodAnimal.csv -dataName GodAnimalData -dataDes %CD%\GodAnimal\GodAnimal_Description_Client.xml -output %CD%\export\GodAnimalData.bin

echo ...Legion...
%DATAEXPORTER% -src %CD%\Legion\LegionContribute.csv -dataName LegionContributeData -dataDes %CD%\Legion\LegionContribute_Description_Client.xml -output %CD%\export\LegionContributeData.bin
%DATAEXPORTER% -src %CD%\Legion\LegionBless.csv -dataName LegionBlessData -dataDes %CD%\Legion\LegionBless_Description_Client.xml -output %CD%\export\LegionBlessData.bin
%DATAEXPORTER% -src %CD%\Legion\LegionCity.csv -dataName LegionCityData -dataDes %CD%\Legion\LegionCity_Description_Client.xml -output %CD%\export\LegionCityData.bin

echo ...Gift...
%DATAEXPORTER% -src %CD%\Legion\Gift.csv -dataName GiftData -dataDes %CD%\Legion\Gift_Description_Client.xml -output %CD%\export\GiftData.bin


echo ...Shop...
%DATAEXPORTER% -src %CD%\Shop\HonorGoods.csv -dataName HonorGoodsData -dataDes %CD%\Shop\HonorGoods_Description_Client.xml -output %CD%\export\HonorGoodsData.bin
%DATAEXPORTER% -src %CD%\Shop\Payment.csv -dataName PaymentData -dataDes %CD%\Shop\Payment_Description_Client.xml -output %CD%\export\PaymentData.bin

echo ...OpenFunction...
%DATAEXPORTER% -src %CD%\Comm\OpenFunction.csv -dataName OpenFunctionData -dataDes %CD%\Comm\OpenFunction_Description_Client.xml -output %CD%\export\OpenFunctionData.bin

echo .....AnnounceMent.....
%DATAEXPORTER% -src %CD%\Comm\AnnounceMent.csv -dataName AnnounceMentData -dataDes %CD%\Comm\AnnounceMent_Description_Client.xml -output %CD%\export\AnnounceMentData.bin

echo .....pushment.......
%DATAEXPORTER% -src %CD%\Comm\PushMent.csv -dataName PushMentData -dataDes %CD%\Comm\PushMent_Description_Client.xml -output %CD%\export\PushMentData.bin

echo ......Task....
%DATAEXPORTER% -src %CD%\Task\Task.csv -dataName TaskData -dataDes %CD%\Task\Task_Description_Client.xml -output %CD%\export\TaskData.bin

echo ......VIP....
%DATAEXPORTER% -src %CD%\Vip\Vip.csv -dataName VipData -dataDes %CD%\Vip\Vip_Description_Client.xml -output %CD%\export\VipData.bin

echo ......VIP....
%DATAEXPORTER% -src %CD%\DreamLand\DreamLandSection.csv -dataName DreamLandSectionData -dataDes %CD%\DreamLand\DreamLandSection_Description_Client.xml -output %CD%\export\DreamLandSectionData.bin

echo ......Manor....
%DATAEXPORTER% -src %CD%\Manor\ManorResLevel.csv -dataName ManorResLevelData -dataDes %CD%\Manor\ManorResLevel_Description_Client.xml -output %CD%\export\ManorResLevelData.bin
%DATAEXPORTER% -src %CD%\Manor\ManorTieJiangPu.csv -dataName ManorTieJiangPuData -dataDes %CD%\Manor\ManorTieJiangPu_Description_Client.xml -output %CD%\export\ManorTieJiangPuData.bin
%DATAEXPORTER% -src %CD%\Manor\ManorWuHunDian.csv -dataName ManorWuHunDianData -dataDes %CD%\Manor\ManorWuHunDian_Description_Client.xml -output %CD%\export\ManorWuHunDianData.bin
%DATAEXPORTER% -src %CD%\Manor\ManorProtect.csv -dataName ManorProtectData -dataDes %CD%\Manor\ManorProtect_Description_Client.xml -output %CD%\export\ManorProtectData.bin

echo ......Arena....
%DATAEXPORTER% -src %CD%\Arena\ArenaAward.csv -dataName ArenaAwardData -dataDes %CD%\Arena\ArenaAward_Description_Client.xml -output %CD%\export\ArenaAwardData.bin

echo ......OperateSystem....
%DATAEXPORTER% -src %CD%\OperateSystem\CheckIn.csv -dataName CheckInData -dataDes %CD%\OperateSystem\CheckIn_Description_Client.xml -output %CD%\export\CheckInData.bin
%DATAEXPORTER% -src %CD%\OperateSystem\OperateActive.csv -dataName OperateActiveData -dataDes %CD%\OperateSystem\OperateActive_Description_Client.xml -output %CD%\export\OperateActiveData.bin
%DATAEXPORTER% -src %CD%\OperateSystem\AccumulatePayment.csv -dataName AccumulatePaymentData -dataDes %CD%\OperateSystem\AccumulatePayment_Description_Client.xml -output %CD%\export\AccumulatePaymentData.bin
%DATAEXPORTER% -src %CD%\OperateSystem\AccumlateLogin.csv -dataName AccumlateLoginData -dataDes %CD%\OperateSystem\AccumlateLogin_Description_Client.xml -output %CD%\export\AccumlateLoginData.bin
%DATAEXPORTER% -src %CD%\OperateSystem\DayPaymentReward.csv -dataName DayPaymentRewardData -dataDes %CD%\OperateSystem\DayPaymentReward_Description_Client.xml -output %CD%\export\DayPaymentRewardData.bin

rem copy /y %RESOURCE_NUMERIC%\export\*.bin %CD%\HttpServer\downloadres > NUL
rem echo ...HttpServer...
rem %DATAEXPORTER% -src %CD%\HttpServer\DownloadList.csv -dataName DownloadListData -dataDes %CD%\HttpServer\DownloadList_Description_Client.xml -output %CD%\export\DownloadListData.bin -md5file %CD%\export\keyfile.md5
rem copy /y %CD%\export\DownloadListData.bin %CD%\HttpServer\downloadres > NUL
rem copy /y %RESOURCE_NUMERIC%\export\*.md5 %CD%\HttpServerData > NUL
rem copy /y %CD%\HttpServer\downloadres\* %CD%\HttpServerData > NUL

copy /y %Numerric%\export\*.bin %Numerric_Expot% > NUL
copy /y %Numerric%\export\*.h %ClientClassPath%\Numeric > NUL
copy /y %Numerric%\export\*.cpp %ClientClassPath%\Numeric > NUL

copy /y %Numerric%\Comm\Global.lua %ClientResPath%\Script > NUL
copy /y %Numerric%\Comm\MusicConfig.lua %ClientResPath%\Script\OtherFunction > NUL
copy /y %Numerric%\Comm\LogoTipCofig.lua %ClientResPath%\Script\OtherFunction >NUL
copy /y %Numerric%\Comm\Dialog.lua %ClientResPath%\Script > NUL
copy /y %Numerric%\Guide\CgConfig.lua %ClientResPath%\Script > NUL
copy /y %Numerric%\Guide\GuideConfig.lua %ClientResPath%\Script > NUL

echo ...Export Numeric for Client Done...


rem echo ...Export Numeric for Download...

rem echo ...Export Numeric for Download Done...


rem echo ...Export Numeric for Server...
rem %PYTHON% %COPY_FILES% %CD% %DATA_SERVER_PATH%\csv ".csv"
rem echo ...Export Numeric for Server Done...