######################################################################################################
# Prostředí distribuovaných aplikací (PDI)
# Projekt - Zpracování IPFIX dat v prostředí Apache Spark
# autori: Lukas Havlicek (xhavli46), Jaroslav Katrusak (xkatru00), Jakub Sadilek (xsadil07)
# readme.txt
######################################################################################################

Stuktura:
    data_ipfix.csv - testovací data s ipfix záznamy z nástroje nfdump (více o získání dat níže)
    source/producer.py - Skript pro načtení csv dat, převedení na json a zaslání pomocí apache kafka
    source/consumer.py - Skript pro čtení dat z apache kafka, jejich zpracování pomocí spark a uložení do databáze cassandra
    congigs/zookeeper.properties - originální config soubor apache kafka pro zookeeper
    congigs/server.properties - originální config soubor apache kafka pro server
    readme.txt

Získání dat:
    data je možné získat pomocí nástroje nfcapd (čtení síťového provozu) nebo pomocí nfpcapd (čte z pcap souborů) (které je možné získat například z https://www.netresec.com/?page=PCAP4SICS)
    jakmile máme k dispozici nfcapd data, můžeme je převést pomocé nfdump na csv "nfdump -r nfcapd.xxxxx -o extended -a -o csv > data_ipfix.csv"

Poslání dat
    data jsou záslána prostřednictvím apache kafka pomocí souboru producer.py
    producer.py [input_file] [topic_name] - pokud jsou argumenty vynechány je použita cesta k souboru "../data_ipfix.csv" a topic "ipfix_"
    je možno spustit jako "klasický" python skript

Zpracování dat
    data jsou získána z apache kafka, zpracována pomocí spark a uložena do DB cassandra pomocí skriptu consumer.py
    consumer.py [topic_name] [keyspace_name] [table_name] - pokud jsou argumenty vynechány je použito "ipfix_", "ks", "ipfix"
    je potřeba spustit pomocí spark utility


Instalace
    je zapotřebí knihoven pro python: kafka-python, pyspark (pip install -r requirements.txt)
    dále je zapotřebí mít nainstalováno apache kafka https://kafka.apache.org/quickstart (2.13-3.0.0)
    a databázi cassandra https://cassandra.apache.org/doc/latest/cassandra/getting_started/installing.html (4.0.1)
    spark je nainstalován jako součást pyspark (verze 3.2.0)


Zprovoznění pipeline
    pro zprovoznění pipeline je zapotřebí běžící kafka a cassandra:
        kafka - je potřeba spustit v adresáří ve kterém ja nainstalována apache kafka (případně pokud je nainstalována globálně, jsou přiloženy originální config soubory ve složce configs)
            bin/zookeeper-server-start.sh config/zookeeper.properties
            bin/kafka-server-start.sh config/server.properties

        cassandra - je potreba spustit v adresáři ve kterém je nainstalována cassandra
            bin/cassandra
            a dále je potřeba vytvořit keyspace a tabulku pro výsledná data pomocí:
            bin/cqlsh
                CREATE KEYSPACE ks WITH replication = {'class': 'SimpleStrategy', 'replication_factor' : 1};
                CREATE TABLE ks.ipfix (
                  key text,
                  sourceIp inet,
                  destIp inet,
                  sourcePort bigint,
                  destPort bigint,
                  protocol text,
                  firstSeen timestamp,
                  lastSeen timestamp,
                  packets bigint,
                  bytes bigint,
                  same_dp list<text>,
                  same_da_dp list<text>,
                  same_sa list<text>,
                  PRIMARY KEY (key));

Spuštění
    - je potřeba být ve složce source
        python producer.py
            - přečte soubor data_ipfix.csv o adresář nížce a zašle jej jako json na topic "ipfix_" na kafka
            - případně je možné spustit jako python producer.py cesta_k_csv_souboru topic
            - vzhledem k tomu, že je využíváno batch streamingu při čtení, data se zasílají 1x před spuštěním consumer.py skriptu

        spark-submit --packages "org.apache.spark:spark-sql-kafka-0-10_2.12:3.2.0,com.datastax.spark:spark-cassandra-connector_2.12:3.1.0" consumer.py
            - přečte data z kafka na topicu "ipfix_", zpracuje je pomocí spark a uloží je do db cassandra do keyspace "ks" s tabulkou "ipfix", případně dle command line argumentů


Výsledná data
    Je možné číst v DB cassandra (například select * from ks.ipfix;)
    Klíč je zvolen jako (Protocol, SrcIp, SrcPort, DstIp, DstPort)
    Jednotlivé hodnoty klíče jsou zde také uloženy jako sloupce, aby s nimi bylo možné pracovat samostatně
    Další jednotlivé sloupce jsou FirstSeen, LastSeen, Bytes, Packets
    A poslední sloupce jsou seznamy korelovaných toků, ve formátu [key,key,key,key....], kde pro každou korelaci je 1 sloupec
        - společný cílový port
        - společná cílová adresa a cílový port
        - společný zdrojová adresa
    (viz schéma cassandra tabulky)
