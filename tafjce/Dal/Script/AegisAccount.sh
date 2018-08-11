#!/bin/sh
DBSQL=real.sql
tbbegin=0
tbend=10
HOST=10.7.11.190
PORT=3306
PWD=''
USR=root

DBNAME=`printf 'db_aegisaccount' `
echo "drop database if EXISTS  ${DBNAME};" >> ${DBSQL}
echo "create database ${DBNAME};" >> ${DBSQL}
mysql -u${USR} -h${HOST} -p${PWD} -P${PORT} < ${DBSQL}
rm ${DBSQL}

DB=`printf 'db_aegisaccount'`
M=$tbbegin
while [ $M -lt $tbend ]
do
	TABLENAME=`printf 't_account_%d' $M`
	echo "drop table if exists ${TABLENAME};"|mysql -u${USR} -h${HOST} -p${PWD} -P${PORT}  ${DB}  >>${DBSQL} 
	echo "CREATE TABLE ${TABLENAME} (
	 	Account varchar(128) NOT NULL DEFAULT '0',
	  CreateTime int(11) NOT NULL DEFAULT '0',
	  Privilege int(11) NOT NULL DEFAULT '0',
	  BanEndTime int(11) NOT NULL DEFAULT '0',
	  LastUptTime int(11) NOT NULL DEFAULT '0',
	  EstopEndTime int(11) NOT NULL DEFAULT '0',
	  PRIMARY KEY (Account)
	) ENGINE=InnoDB DEFAULT CHARSET=latin1" |mysql -u${USR} -h${HOST} -p${PWD} -P${PORT} ${DB}  >> ${DBSQL}
	let M=$M+1
done

#创建账号福利表
TABLENAME=`printf 't_accbenifit'`
echo "drop table if exists ${TABLENAME};"|mysql -u${USR} -h${HOST} -p${PWD} -P${PORT}  ${DB}  >>${DBSQL} 
echo "CREATE TABLE ${TABLENAME} (
	  ID 			int(11) unsigned NOT NULL AUTO_INCREMENT,	
	  Account 		varchar(128) 	DEFAULT '',
	  Name 			varchar(41) 	DEFAULT '',
	  ExchgID 		int(11) unsigned NOT NULL DEFAULT '0',
	  ExchgInfo 	varchar(25) 	NOT NULL DEFAULT '0',
	  RefreshType 	int(3) NOT NULL DEFAULT '0',
	  EndTime 		int(11) unsigned NOT NULL DEFAULT '0',
	  AwardTime 	int(11) unsigned NOT NULL DEFAULT '0',	  
	  PRIMARY KEY (ID),
	  UNIQUE KEY IDXA (Account,ExchgID)
	) ENGINE=InnoDB DEFAULT CHARSET=latin1" |mysql -u${USR} -h${HOST} -p${PWD} -P${PORT} ${DB}  >> ${DBSQL}

rm ${DBSQL}
