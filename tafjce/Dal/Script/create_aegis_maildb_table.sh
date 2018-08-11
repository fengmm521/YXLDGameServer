#!/bin/sh

#20120804:kirkdai first add

# 数据库连接
DB_CONN="-uroot"

# 数据库
DB_MAIL_PRE="db_aegismail_"

# 数据表

T_MAIL_PRE="t_mail_"   # 用户邮件

# 数据字段
C_MAIL_PRE="
    owner_name    varchar(31) NOT NULL,
    mail_id       int(11) UNSIGNED NOT NULL DEFAULT '0',
    mailbox_type  tinyint(4) NOT NULL DEFAULT '0',
    mail_type     tinyint(4) NOT NULL DEFAULT '0',
    mail_stat     tinyint(4) NOT NULL DEFAULT '0',
    mail_time     int(11) UNSIGNED NOT NULL DEFAULT '0',
    sender_name   varchar(31) NOT NULL,
    recver_name   varchar(31) NOT NULL DEFAULT '',
    mail_title    varchar(37) NOT NULL,
    mail_content  varchar(801),
    attach_List   blob, 
    PRIMARY KEY (owner_name,mail_id)"

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


create_100_aegis_db_table()
{
    for((i=0; i<10; i++))
    do
        database_name=`printf "${DB_MAIL_PRE}%02d" $i`
        create_database $database_name

        for ((j=0;j<10;j++))
        do
            table_name=`printf "${T_MAIL_PRE}%d" $j`
            create_table $database_name $table_name "$C_MAIL_PRE"
        done
    done
}

create_100_aegis_db_table
