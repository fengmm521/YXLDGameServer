#!/bin/bash
DBSQL=real.sql
tbbegin=0
tbend=5
HOST=127.0.0.1
PORT=3306
PWD='mysql'
USR=root

DBNAME=`printf 'db_legionname' `
echo "drop database if EXISTS  ${DBNAME};" >> ${DBSQL}
echo "create database ${DBNAME};" >> ${DBSQL}
mysql -u${USR} -h${HOST} -p${PWD} -P${PORT} < ${DBSQL}
rm ${DBSQL}

DB=`printf 'db_legionname'`
M=$tbbegin
while [ $M -lt $tbend ]
do
	TABLENAME=`printf 't_name%d' $M`
	echo "drop table if exists ${TABLENAME};"|mysql -u${USR} -h${HOST} -p${PWD} -P${PORT}  ${DB}  >>${DBSQL} 
	echo "CREATE TABLE ${TABLENAME} (
	  Name varchar(128) NOT NULL DEFAULT '0',
	  data blob,
	  PRIMARY KEY (Name)
	) ENGINE=InnoDB DEFAULT CHARSET=latin1" |mysql -u${USR} -h${HOST} -p${PWD} -P${PORT} ${DB}  >> ${DBSQL}
	let M=$M+1
done

rm ${DBSQL}
