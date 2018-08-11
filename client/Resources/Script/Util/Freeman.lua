-- 获取实体对象
function GetEntityById( entityId, className )
	local entity = nil;
	if( type(entityId) == "string" )then
		entity = EntityMgr:GetInstance():GetEntityByIdStr( entityId );
	else
		-- uint64?
		entity = EntityMgr:GetInstance():GetEntityById( entityId );
	end
	if( entity ~= nil and className ~= nil )then
		return tolua.cast( entity, className );
	end
	return entity;
end



-- 获取组件
function GetComponent( entity, componentId, className )
	local component = entity:GetComponentById( componentId );
	if( component == nil )then
		return nil;
	end
	return tolua.cast( component, className );
end
