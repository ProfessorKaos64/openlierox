#!/bin/sh

CURDATE=`date "+%Y-%m-%d_%H:%M"`

# Clean up stale processes, or it will easily eat everything and you won't be able to enter the host by ssh
rm -rf /tmp/rMD-session-* video-*.ogv
killall -KILL recordmydesktop jackd Xvfb
DO_NOT_KILL="`cat ded_main_pids.pid` $$"
ALL_PIDS="`pgrep -U $USER 'openlierox|dedicated_contr|dedicated-video-record'`"
PIDS_TO_KILL=
#echo ALL_PIDS $ALL_PIDS
#echo DO_NOT_KILL $DO_NOT_KILL
for PID in $ALL_PIDS; do
	#echo CHECK $PID
	NO_KILL=`echo $DO_NOT_KILL | egrep \\\b$PID\\\b`
	#echo NO_KILL $NO_KILL
	if [ -z "$NO_KILL" ] ; then
		PIDS_TO_KILL="$PIDS_TO_KILL $PID"
	fi
	#echo PIDS_TO_KILL $PIDS_TO_KILL
done
echo Killing stale processes $PIDS_TO_KILL
for PID in $PIDS_TO_KILL; do kill -KILL $PID ; done

jackd -d dummy &
JOBS=$!
sleep 1
#Xvfb somehow catches Ctrl-C bypassing our trap (reads TTY perhaps?), so run it in different session and kill with killall
setsid Xvfb :11 -screen 0 640x480x16 &
XVFB=$!
JOBS="$JOBS $XVFB"
sleep 1
# jacklaunch is a simple script which messes up quoted arguments, so we're just doing LD_PRELOAD
env DISPLAY=:11.0 SDL_AUDIODRIVER=dsp LD_PRELOAD=/usr/lib/libjackasyn.so.0 \
../../bin/openlierox \
-exec "setVar GameOptions.Advanced.MaxFPS 12" \
-exec "setVar GameOptions.Game.LastSelectedPlayer ''" \
-exec "setVar GameOptions.Audio.Enabled 1" \
-exec "connect 127.0.0.1" \
-exec "wait game chatMsg /spectate ; chatMsg /suicide ; setViewport actioncam" \
&
OLX=$!
sleep 5
# Warning: Debian includes recordmydesktop with no Jack support, you'll have to compile it yourself
# --on-the-fly-encoding eats CPU, and --compress-cache too
# Nice it a bit, so it won't slow down ded server itself
nice -n 2 \
env DISPLAY=:11.0 \
recordmydesktop -o video-$$.ogv --no-cursor --fps 12 --v_quality 30 --s_quality 3 --on-the-fly-encoding \
--use-jack `jack_lsp | grep openlierox | head -n 1` --no-frame --overwrite &
RECORD=$!

#TODO: youtube upload, use scripts at http://code.google.com/p/gdata-python-client/downloads/list
# and http://code.google.com/apis/youtube/1.0/developers_guide_python.html#DirectUpload

UPLOADCMD="python YoutubeUpload.py OpenLieroXVideoBot01 UploadVideo357236 video-$$.ogv $CURDATE"
# Send INT signal or Ctrl-C when you done
trap "kill -INT $RECORD ; kill -INT $OLX ; wait $OLX $RECORD ; kill -INT $JOBS ; \
sleep 1 ; wait ; $UPLOADCMD ; rm video-$$.ogv" INT QUIT

wait
