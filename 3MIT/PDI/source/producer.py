######################################################################################################
# PDI - Projekt (Zpracování IPFIX dat v prostředí Apache Spark)
# autori: Lukas Havlicek (xhavli46), Jaroslav Katrusak (xkatru00), Jakub Sadilek (xsadil07)
# Soubor, ktery cte csv soubor s daty a posila je pres kafku
#
######################################################################################################


from json import dumps
from kafka import KafkaProducer
import csv
import sys


# creating kafka producer
producer = KafkaProducer(bootstrap_servers=['localhost:9092'],
                         value_serializer=lambda x:
                         dumps(x).encode('utf-8'))

# 1st command line argument is path to file to read
file_name = sys.argv[1] if len(sys.argv) >= 2 else '../data_ipfix.csv'
# 2nd command line argument is name of topic
topic = sys.argv[2] if len(sys.argv) >= 3 else 'ipfix_'

def all_data():
    # reading dat from csv
    with open(file_name, 'r') as file:
        # parsing the csv to dict (so it can be used as json)
        reader = csv.DictReader(file, delimiter=',')
        jsonArray = []
        for messages in reader:
            # skipping last lines, that contains summary of all data
            if messages['ts'] == "Summary":
                break
            # adding row to array
            jsonArray.append(messages)

        # sending data using kafka
        producer.send(topic, value=jsonArray)
        # making sure the data are sent
        producer.flush()


all_data()

