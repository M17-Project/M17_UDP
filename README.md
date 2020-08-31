# M17_UDP
Receives voice packets via UDP and allows to play them back using aplay.  
  
# Usage
./m17_udp | aplay -t raw -f S16_LE -r 8000 -c 1
or maybe
./m17_udp | aplay -t raw -f S16_BE -r 8000 -c 1
