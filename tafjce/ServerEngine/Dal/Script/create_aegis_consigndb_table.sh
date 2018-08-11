#!/bin/sh


# 数据库连接
DB_CONN="-uroot"

# 数据库
DB_ARENA_PRE="db_aegisconsign_"

# 数据表

T_CONSIGN_SALE="t_consign_sale"                     # 拍卖列表
T_CONSIGN_BUY="t_consign_buy"                       # 求购列表
T_CONSIGN_SALE_EXPIRED="t_consign_sale_expired"     # 拍卖列表己过期还未退货表,列结构与T_CONSIGN_SALE保持一致
T_CONSIGN_BUY_EXPIRED="t_consign_buy_expired"       # 求购列表己过期还未退货表,列结构与T_CONSIGN_BUY保持一致
T_CONSIGN_MISC="t_consign_misc"                     # 杂项记录:包括历史求购成功量、拍卖历史量、拍卖热门搜索词汇

# 数据字段
C_CONSIGN_SALE="
    trade_id               int(11) UNSIGNED NOT NULL DEFAULT '0',    
    sale_role_account      varchar(128) NOT NULL,
    sale_role_pos          int(11) UNSIGNED NOT NULL DEFAULT '0',
    sale_role_name         varchar(61) NOT NULL,
    obj_container_type     int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_baseid             int(11) UNSIGNED NOT NULL DEFAULT '0',
	obj_name               varchar(32) NOT NULL,
	obj_count              int(11) UNSIGNED NOT NULL DEFAULT '0',    
    obj_role_uselevel      int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_quality            int(11) UNSIGNED NOT NULL DEFAULT '0',
    pack_obj               blob,
    obj_class_lvl_1        int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_class_lvl_2        int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_class_lvl_3        int(11) UNSIGNED NOT NULL DEFAULT '0', 
    sale_price             int(11) UNSIGNED NOT NULL DEFAULT '0',
    sale_moneytype         int(11) UNSIGNED NOT NULL DEFAULT '0',
    onshelf_time           int(11) UNSIGNED NOT NULL DEFAULT '0',  
    obj_hisale_count       int(11) UNSIGNED NOT NULL DEFAULT '0',  
    trade_uuid             varchar(128) NOT NULL,  
    update_time            timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
    PRIMARY KEY (trade_id)"
    
C_CONSIGN_BUY="
    trade_id               int(11) UNSIGNED NOT NULL DEFAULT '0',    
    buy_role_account       varchar(128) NOT NULL,
    buy_role_pos           int(11) UNSIGNED NOT NULL DEFAULT '0',
    buy_role_name          varchar(61) NOT NULL,
    obj_baseid             int(11) UNSIGNED NOT NULL DEFAULT '0',
	obj_name               varchar(32) NOT NULL,
	obj_count              int(11) UNSIGNED NOT NULL DEFAULT '0',    
    obj_role_uselevel      int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_quality            int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_class_lvl_1        int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_class_lvl_2        int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_class_lvl_3        int(11) UNSIGNED NOT NULL DEFAULT '0', 
    buy_price              int(11) UNSIGNED NOT NULL DEFAULT '0',
    buy_moneytype          int(11) UNSIGNED NOT NULL DEFAULT '0',
    onshelf_time           int(11) UNSIGNED NOT NULL DEFAULT '0', 
    obj_hisbuy_count       int(11) UNSIGNED NOT NULL DEFAULT '0',
    obj_left_count         int(11) UNSIGNED NOT NULL DEFAULT '0',
    trade_uuid             varchar(128) NOT NULL,  
    update_time            timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
    PRIMARY KEY (trade_id)"    

C_CONSIGN_MISC="
    para_id               int(11) UNSIGNED NOT NULL DEFAULT '0',
    para_name             varchar(128) NOT NULL,    
    data_blob             mediumblob,
    update_time           timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
    PRIMARY KEY (para_id)"    

create_database()
{
    echo "DROP DATABASE $1;"
    mysql $DB_CONN -e "DROP DATABASE IF EXISTS $1;"
    echo "CREATE DATABASE $1;"
    mysql $DB_CONN -e "CREATE DATABASE $1 DEFAULT CHARSET=latin1;"
}

create_table()
{
    echo "   CREATE TABLE $2;"
    mysql $DB_CONN $1 -e "CREATE TABLE $2 ($3) ENGINE=InnoDB DEFAULT CHARSET=latin1;"
}


create_aegis_db_table()
{ 
	for((i=0; i<7; i++))
    do
        database_name=`printf "${DB_ARENA_PRE}%02d" $i`
        create_database $database_name

		table_name=$T_CONSIGN_SALE
		create_table $database_name $table_name "$C_CONSIGN_SALE"
		
		table_name=$T_CONSIGN_SALE_EXPIRED
		create_table $database_name $table_name "$C_CONSIGN_SALE"
		
		table_name=$T_CONSIGN_BUY
		create_table $database_name $table_name "$C_CONSIGN_BUY"
		
		table_name=$T_CONSIGN_BUY_EXPIRED
		create_table $database_name $table_name "$C_CONSIGN_BUY"
		
		table_name=$T_CONSIGN_MISC
		create_table $database_name $table_name "$C_CONSIGN_MISC"
    done
}

create_aegis_db_table
