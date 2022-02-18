######################################################################################################
# PDI - Projekt (Zpracování IPFIX dat v prostředí Apache Spark)
# autori: Lukas Havlicek (xhavli46), Jaroslav Katrusak (xkatru00), Jakub Sadilek (xsadil07)
# Soubor, ktery cte data z kafky, nasledne data zpracuje a ulozi do DB cassandra (kde jiz ocekava hotovou tabulku)
#
######################################################################################################

import json

from pyspark.sql import SparkSession
from pyspark.sql.functions import *
from pyspark.sql.types import *
import sys

# 1st command line argument is name of topic
topic = sys.argv[1] if len(sys.argv) >= 2 else 'ipfix_'
# 2nd command line argument is existing keyspace in cassandra
keyspace = sys.argv[2] if len(sys.argv) >= 3 else 'ks'
# 3rd command line argument is existing tablename in cassandra
tablename = sys.argv[3] if len(sys.argv) >= 4 else 'ipfix'

# creating sparkSession
spark = SparkSession \
    .builder \
    .appName("APP") \
    .getOrCreate()

# only getting WARN logs (so the console isn't flooded)
spark.sparkContext.setLogLevel('WARN')


# defining spark (that will read from kafka as batch stream)
df = spark \
    .read \
    .format("kafka") \
    .option("kafka.bootstrap.servers", "localhost:9092") \
    .option("subscribe", topic) \
    .option("startingOffsets", "earliest") \
    .option("endingOffsets", "latest") \
    .load()

# definining schema for parsing from kafka (all data are in array in a struct as in csv from nfdump)
schema1 = ArrayType(StringType())
schema2 = StructType([
    StructField("ts", StringType()),
    StructField("te", StringType()),
    StructField("td", StringType()),
    StructField("sa", StringType()),
    StructField("da", StringType()),
    StructField("sp", StringType()),
    StructField("dp", StringType()),
    StructField("pr", StringType()),
    StructField("flg", StringType()),
    StructField("fwd", StringType()),
    StructField("stos", StringType()),
    StructField("ipkt", StringType()),
    StructField("ibyt", StringType()),
    StructField("opkt", StringType()),
    StructField("obyt", StringType()),
    StructField("in", StringType()),
    StructField("out", StringType()),
    StructField("sas", StringType()),
    StructField("das", StringType()),
    StructField("smk", StringType()),
    StructField("dmk", StringType()),
    StructField("dtos", StringType()),
    StructField("dir", StringType()),
    StructField("nh", StringType()),
    StructField("nhb", StringType()),
    StructField("svln", StringType()),
    StructField("dvln", StringType()),
    StructField("ismc", StringType()),
    StructField("odmc", StringType()),
    StructField("idmc", StringType()),
    StructField("osmc", StringType()),
    StructField("mpls1", StringType()),
    StructField("mpls2", StringType()),
    StructField("mpls3", StringType()),
    StructField("mpls4", StringType()),
    StructField("mpls5", StringType()),
    StructField("mpls6", StringType()),
    StructField("mpls7", StringType()),
    StructField("mpls8", StringType()),
    StructField("mpls9", StringType()),
    StructField("mpls10", StringType()),
    StructField("cl", StringType()),
    StructField("sl", StringType()),
    StructField("al", StringType()),
    StructField("ra", StringType()),
    StructField("eng", StringType()),
    StructField("exid", StringType()),
    StructField("tr", StringType())
])


# parsing data from kafka using schema
query = df\
    .select(from_json(col("value").cast("string"), schema1).alias("parsed_value"))\
    .withColumn("data", explode(col("parsed_value")))\
    .select("data")

# parsing data from struct and selecting only useful data
query1 = query\
    .select(from_json(col("data").cast("string"), schema2).alias("parsed_value"))\
    .select("parsed_value.*").select("sa","da","sp","dp","pr","ts","te","ipkt","ibyt")

# merging protocol, src address, dest address, src port and dst port into 1 column key
query2 = query1\
    .select("*", concat(lit("("),col("pr"),lit(","),col("sa"),lit(","),col("sp"),lit(","),col("da"),lit(","),col("dp"),lit(")")).alias("key"))
# query2.show(20, False)

# group by source address
same_src_add = query2\
    .groupBy("sa")\
    .agg(collect_list('key').alias('same_sa'))
# same_src_add.show(20, False)

# group by destination port
same_port = query2\
    .groupBy("dp")\
    .agg(collect_list('key').alias('same_dp'))
# same_port.show(20, False)

# group by destination address a destination port
same_da_dp = query2\
    .groupBy("da","dp")\
    .agg(collect_list('key')
         .alias('same_da_dp'))
# same_da_dp.show(20, False)


# joining with same destination port and destination address and dropping duplicate columns that are added through join
fin = query2\
    .join(same_da_dp, (query2.da == same_da_dp.da) & (query2.dp == same_da_dp.dp), "inner")\
    .drop(same_da_dp.da)\
    .drop(same_da_dp.dp)

# joining with same destination port and dropping duplicate columns that are added through join
fin = fin\
    .join(same_port, fin.dp == same_port.dp, "inner")\
    .drop(same_port.dp)

# joining with same source address and dropping duplicate columns that are added through join
fin = fin\
    .join(same_src_add, fin.sa == same_src_add.sa, "inner")\
    .drop(same_src_add.sa)

# renaming columns to match created DB table (which are more readable)
fin = fin\
    .withColumnRenamed("sa", "sourceip")\
    .withColumnRenamed("da", "destip")\
    .withColumnRenamed("sp", "sourceport")\
    .withColumnRenamed("dp", "destport")\
    .withColumnRenamed("pr", "protocol")\
    .withColumnRenamed("ts", "firstseen")\
    .withColumnRenamed("te", "lastseen")\
    .withColumnRenamed("ipkt", "packets")\
    .withColumnRenamed("ibyt", "bytes")

# fin.show(20, False)

# writing data to cassandra
fin\
    .write\
    .mode("append")\
    .format("org.apache.spark.sql.cassandra")\
    .options(table="ipfix", keyspace="ks")\
    .save()




