#!/bin/sh

#20120804:kirkdai first add

# 数据库连接
DB_CONN="-uroot"

# 数据库
DB_NAME_PRE="db_aegis_familyname"

# 数据表

T_NAME_PRE="t_familyname_"   # 用户名

# 数据字段
C_NAME_PRE="
  name varchar(21) CHARACTER SET latin1 COLLATE latin1_bin NOT NULL,
  data blob,
  PRIMARY KEY (name)"

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
    
    database_name=$DB_NAME_PRE
    create_database $database_name

    for ((j=0;j<10;j++))
    do
        table_name=`printf "${T_NAME_PRE}%d" $j`
        create_table $database_name $table_name "$C_NAME_PRE"
    done
}

create_aegis_db_table
