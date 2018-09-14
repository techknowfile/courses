import psycopg2
import time

DATABASE_NAME = 'dds_assgn1'
RANGE_TABLE_PREFIX = 'range_part'
RROBIN_TABLE_PREFIX = 'rrobin_part'
RROBIN_METADATA_TABLE = 'rrobin_metadata'
RANGE_METADATA_TABLE = 'range_metadata'

def getopenconnection(user='postgres', password='1234', dbname='postgres'):
    return psycopg2.connect("dbname='" + dbname + "' user='" + user + "' host='localhost' password='" + password + "'")

def loadratings(ratingstablename, ratingsfilepath, openconnection):
    cur = openconnection.cursor()
    cur.execute("CREATE TABLE {} (userid integer, movieid integer, rating float, time integer);".format(ratingstablename))
    with open(ratingsfilepath, 'r') as f:
        for line in f:
            tup = line.split("::")
            tup = tuple(tup)
            cur.execute("INSERT INTO {} VALUES ({}, {}, {}, {});".format(ratingstablename, *tup))

def rangepartition(ratingstablename, numberofpartitions, openconnection):
    cur = openconnection.cursor()
    increment = 5 / numberofpartitions
    lower_limit = -1
    cur.execute("CREATE TABLE IF NOT EXISTS {} (name VARCHAR(40) NOT NULL, partition INTEGER, lower FLOAT, upper FLOAT, PRIMARY KEY (name, partition));".format(RANGE_METADATA_TABLE))
    for partition in range(numberofpartitions):
        createTableString = "CREATE TABLE {}{} (LIKE {} INCLUDING ALL);".format(RANGE_TABLE_PREFIX, partition, ratingstablename)
        cur.execute(createTableString)
        cur.execute("INSERT INTO {}{} SELECT * FROM {} WHERE rating > {} AND RATING <= {}".format(RANGE_TABLE_PREFIX, partition, ratingstablename, lower_limit, (partition + 1)*increment))
        cur.execute("INSERT INTO {} VALUES ('{}', {}, {}, {});".format(RANGE_METADATA_TABLE, ratingstablename, partition, lower_limit, (partition + 1) * increment))
        lower_limit = (partition + 1) * increment
    cur.close()

def roundrobinpartition(ratingstablename, numberofpartitions, openconnection):
    cur = openconnection.cursor()
    for partition in range(numberofpartitions):
        partitionTableName = RROBIN_TABLE_PREFIX + str( partition )
        createTableString = "CREATE TABLE {} (LIKE {} INCLUDING ALL);".format(partitionTableName, ratingstablename)
        cur.execute(createTableString)

        cur.execute("""
                    INSERT INTO {} (
                        SELECT temp.userid, temp.movieid, temp.rating, temp.time
                        FROM (
                            SELECT *, -1 + row_number() OVER(ORDER BY time ASC) as row
                            FROM {}
                        ) temp
                        WHERE temp.row % {} = {}
                    );
                    """.format(partitionTableName, ratingstablename, numberofpartitions, partition))
    cur.execute("SELECT COUNT(*) FROM {};".format(ratingstablename))
    rrobin_idx = cur.fetchone()[0] % 5
    cur.execute("CREATE TABLE IF NOT EXISTS {} (name VARCHAR(40) NOT NULL, insert_idx integer);INSERT INTO {} VALUES ('{}', {});".format(RROBIN_METADATA_TABLE, RROBIN_METADATA_TABLE, ratingstablename, rrobin_idx))
    cur.close()
    
def roundrobininsert(ratingstablename, userid, itemid, rating, openconnection):
    cur = openconnection.cursor()
    cur.execute("SELECT insert_idx FROM {} WHERE name = '{}';".format(RROBIN_METADATA_TABLE, ratingstablename))
    rrobin_metadata = cur.fetchone()
    if not rrobin_metadata:
        print "No round robin metadata found for the table specified"
        return
    insert_idx = rrobin_metadata[0]
    
    current_time = int(time.time())
    cur.execute("""
                BEGIN;
                INSERT INTO {} VALUES ({}, {}, {}, {});
                UPDATE {} SET insert_idx = insert_idx + 1
                WHERE name = '{}';
                COMMIT;"""
                .format(RROBIN_TABLE_PREFIX + str(insert_idx), userid, itemid, rating, current_time,
                        RROBIN_METADATA_TABLE, ratingstablename))

def rangeinsert(ratingstablename, userid, itemid, rating, openconnection):
    cur = openconnection.cursor()
    cur.execute("SELECT name, partition, lower, upper from {} WHERE name = '{}' AND lower < {} AND upper >= {};".format(RANGE_METADATA_TABLE, ratingstablename, rating, rating))
    result = cur.fetchone()
    if not result:
        print "ERROR: No range partition found appropriate for the rating provided!"
        return
    partition = result[1]
    current_time = int( time.time() )
    cur.execute("INSERT INTO {} VALUES ({}, {}, {}, {})".format(RANGE_TABLE_PREFIX + str(partition), userid, itemid, rating, current_time))
