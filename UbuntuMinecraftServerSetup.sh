echo "Install updates to the guest"
sudo apt-get update
sudo apt-get --yes upgrade

echo "Install guest additions to the virtual machine"
sudo apt-get --yes install virtualbox-guest-dkms
echo "Make ourselves a user visible to the host"
sudo usermod -aG vboxsf jkumley

echo "Install java"
sudo add-apt-repository --yes ppa:webupd8team/java
sudo apt update; sudo apt --yes install oracle-java8-installer
java -version; javac -version
sudo apt --yes install oracle-java8-set-default

echo "Make sure we have the downloader"
sudo apt-get --yes install wget
echo "Download the jar file"
wget https://s3.amazonaws.com/Minecraft.Download/versions/1.11.2/minecraft_server.1.11.2.jar
echo "Make the file executable"
chmod +x minecraft_server.1.11.2.jar

echo "Accept the terms"
echo "eula=true" > eula.txt

echo "Run the server to create the default files"
echo "java -Xmx8192M -Xms8192M -jar minecraft_server.1.11.2.jar nogui" > run
chmod +x run
./run
stop

echo "Find the IP address"
echo "ifconfig | grep -m 1 'inet addr' | cut -f2 -d':' | cut -f1 -d' '" > ip
chmod +x ip
./ip

echo "Good to go!"
