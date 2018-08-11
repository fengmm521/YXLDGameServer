#!/bin/sh

#20120828:kirkdai first add

# 数据库连接
DB_CONN="-uroot"

# 数据库
DB_ARENA_PRE="db_aegisarena_"

# 数据表

T_ARENA_BASE="t_arena_rank"   # 竞技场排行列表
T_ARENA_BAK_PRE="t_arena_awardbak_"   # 竞技场排行列表备份

# 数据字段
C_ARENA="
    arena_rank        int(11) UNSIGNED NOT NULL,
    role_account      varchar(128) NOT NULL,
    role_pos          int(11) UNSIGNED NOT NULL,
    role_name         varchar(61) NOT NULL,
    role_lvl          int(11) UNSIGNED NOT NULL,
    role_sex          int(11) UNSIGNED NOT NULL,
    role_job          int(11) UNSIGNED NOT NULL,
    record_time       int(11) UNSIGNED NOT NULL,
    role_maxhp        int(11) UNSIGNED NOT NULL,
    arena_exthp       int(11) NOT NULL,
    last_active_time  int(11) NOT NULL DEFAULT 0,
    last_gotaward_time  int(11) NOT NULL DEFAULT 0,
    PRIMARY KEY (arena_rank)"

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
    for((i=0; i<10; i++))
    do
	for((k=0; k<4; k++))
	do
		database_name=`printf "${DB_ARENA_PRE}%02d_%d" $i $k`
		create_database $database_name

		table_name=$T_ARENA_BASE
		create_table $database_name $table_name "$C_ARENA"
			
		for ((j=1;j<=31;j++))
		do
		    table_name=`printf "${T_ARENA_BAK_PRE}%02d" $j`
		    create_table $database_name $table_name "$C_ARENA"
		done
	done
    done
}

create_aegis_db_table
