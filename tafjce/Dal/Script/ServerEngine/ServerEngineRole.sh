#!/bin/bash
DBSQL=real.sql
dbbegin=0
dbend=10
tbbegin=0
tbend=10
HOST=127.0.0.1
PORT=3306
PWD='mysql'
USR=root

while [ $dbbegin -lt $dbend ]
do
  id=`expr $dbbegin % $dbend`
  DBNAME=`printf 'db_aegis_%02d' $dbbegin`
  echo "drop database if EXISTS  ${DBNAME};" >> ${DBSQL}
	echo "create database ${DBNAME};" >> ${DBSQL}
	dbbegin=`expr $dbbegin + 1`
done

mysql -u${USR} -h${HOST} -p${PWD} -P${PORT}  < ${DBSQL}
rm ${DBSQL}

dbbegin=0
N=$dbbegin
while [ $N -lt $dbend ]
do
	DB=`printf 'db_aegis_%02d' $N`
	M=$tbbegin
	while [ $M -lt $tbend ]
	do
		TABLENAME=`printf 't_role_%d' $M`
		echo "drop table if exists ${TABLENAME};"|mysql -u${USR} -h${HOST} -p${PWD} -P${PORT}  ${DB}  >>${DBSQL} 
		echo "CREATE TABLE ${TABLENAME} (
		ID int(11) UNSIGNED NOT NULL AUTO_INCREMENT,
		Account varchar(128) NOT NULL,
		Pos tinyint(4) NOT NULL DEFAULT '-1',
		data blob,
		PRIMARY KEY (ID), UNIQUE KEY (Account,Pos)) ENGINE=InnoDB DEFAULT CHARSET=latin1 " |mysql -u${USR} -h${HOST} -p${PWD} -P${PORT} ${DB}  >> ${DBSQL}
		let M=$M+1
	done
	
    let N=$N+1
done
rm ${DBSQL}
