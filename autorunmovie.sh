MOVIE_PATH=/home/dream/Android_Lowpower_project/Movie/480_360_4/
for movie_num in 2
do

  ./ffplay -autoexit -skip_frame none ${MOVIE_PATH}${movie_num}.mp4
done
