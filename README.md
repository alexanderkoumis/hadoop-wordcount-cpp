# Hadoop C++ WordCount

This project contains information regarding compiling the C++ WordCount implementation found [here][wordcount_link] with Hadoop 2.6, as the original guide (and many other Hadoop C++ tutorials) shows deprecated Hadoop binary calls, and a makefile that will not compile with modern versions of GCC. Most execution instructions are derived from [Setting up a Single Node Cluster][apache_hadoop_doc].

[wordcount_link]: http://cs.smith.edu/dftwiki/index.php/Hadoop_Tutorial_2.2_--_Running_C%2B%2B_Programs_on_Hadoop
[apache_hadoop_doc]: https://hadoop.apache.org/docs/stable/hadoop-project-dist/hadoop-common/SingleCluster.html

## Install Hadoop

The most trivial way to get Hadoop up and running is to start a Docker container with Apache's Hadoop 2.6.0 Docker image, based on Ubuntu 14.04. These instructions assume your development machine is also running Ubuntu 14.04. Skip to the next section if your Hadoop is already set up, or you would like to configure it manually.

First [install Docker][docker_link] for your respective platform.

Ensure the current user is part of the docker group in order to run docker commands without sudo:

```bash
sudo groupadd docker
sudo gpasswd -a ${USER} docker
sudo service docker restart
sudo reboot -f now
```

Then, pull and run the docker-hadoop-ubuntu image:

```bash
docker pull sequenceiq/hadoop-ubuntu:2.6.0
docker run -i -t sequenceiq/hadoop-ubuntu:2.6.0 /etc/bootstrap.sh -bash
```

Your shell should now be at a root prompt from within the Docker image, pre-loaded with Hadoop.

[docker_link]: https://docs.docker.com/installation/

## Compile WordCount

Update the instance's repos and install libssl-dev and git:

```bash
apt-get update && apt-get install libssl-dev git -y
```

Clone this project into $HADOOP_PREFIX and build:

```bash
cd $HADOOP_PREFIX
git clone https://github.com/alexanderkoumis/hadoop-wordcount-cpp wordcount
cd wordcount
make -j4
```

## Initialize Hadoop, Run WordCount

Start the Hadoop filesystem, create a user directory for root, and copy the WordCount binary into the dfs:

```bash
cd $HADOOP_PREFIX
bin/hdfs namenode -format
sbin/start-dfs.sh
bin/hdfs dfs -put wordcount input
```

(Note: It is assumed the rest of the supplied commands are executed from working directory $HADOOP_PREFIX)

Finally run WordCount on the supplied 2015 State of the Union address:

```bash
bin/mapred pipes -D hadoop.pipes.java.recordreader=true \
                 -D hadoop.pipes.java.recordwriter=true \
                 -input input/wordcount/sotu_2015.txt   \
                 -output output                         \
                 -program input/wordcount/wordcount
```

To see the results:

```bash
bin/hdfs dfs -cat output/part-00000
```

When finished, close daemons with:

```bash
sbin/stop-dfs.sh
```

## Tips / Common Errors

Hostname of namenode GUI: `<docker instance ip>:50070` (default)
 
Most important config files are in: `/usr/local/hadoop/etc/hadoop`

The 'I-have-no-idea-whats-wrong-so-im-just-going-to-nuke-it' script:

```bash
#!/bin/bash
cd $HADOOP_PREFIX
sbin/stop-dfs.sh
rm -rf /tmp/hadoop-root/dfs/data/ /tmp/hadoop-root/dfs/name
bin/hdfs namenode -format
sbin/start-dfs.sh
bin/hdfs dfs -mkdir /user
bin/hdfs dfs -mkdir /user/root
```

If the system complains about 'Retrying to connect to server' when attempting to start a job, `ResourceManager` is most likely not running. Confirm by typing `jps` to list running Java processes. To start ResourceManager, type: `sbin/start-yarn.sh'
