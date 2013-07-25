/*
 * TestObjects.cpp
 *
 *  Created on: Jul 2, 2013
 *      Author: zachary
 */

#include "TestObjects.h"

using namespace std;

TestObjects::TestObjects() {


}

TestObjects::~TestObjects() {
	// TODO Auto-generated destructor stub
}

void TestObjects::InsertTestMovie(CObjectDatabase& db)
{
	CStdString movieParent = "/home/zach/movies/";
	db.AddPath(movieParent);

	CStdString movie1File = movieParent + "Star_Wars_(1977)/movie.mkv";
	CStdString movie2File = movieParent + "Star_Wars_V_(1980)/movie.mkv";
	CStdString movie3File = movieParent + "Star_Wars_VI_(1983)/movie.mkv";

	int movie1DirEnt = db.AddDirEnt(movie1File);
	int movie2DirEnt = db.AddDirEnt(movie2File);
	int movie3DirEnt = db.AddDirEnt(movie3File);

	int movie1Obj = db.AddObject(MOVIE, "StarWars4", "Star Wars: Episode IV - A New Hope");
	int movie2Obj = db.AddObject(MOVIE, "StarWars5", "Star Wars: Episode V - The Empire Strikes Back");
	int movie3Obj = db.AddObject(MOVIE, "StarWars6", "Star Wars: Episode VI - Return of the Jedi");

	db.LinkObjectToDirent(movie1Obj, movie1DirEnt);
	db.LinkObjectToDirent(movie2Obj, movie2DirEnt);
	db.LinkObjectToDirent(movie3Obj, movie3DirEnt);

	CAttributeType fileName;
	CAttributeType plot;
	CAttributeType contentRating;
	CAttributeType onlineRating;

	db.GetAttributeType(FILENAME, fileName);
	db.GetAttributeType(MOVIE_PLOT, plot);
	db.GetAttributeType(CONTENTRATING, contentRating);
	db.GetAttributeType(ONLINERATING, onlineRating);

	map<int, CAttribute> movie1Attributes;

	CAttribute movie1fileName = CAttribute(fileName);
	movie1fileName.setStringValue(movie1File);
	movie1Attributes.insert(make_pair(fileName.idAttributeType, movie1fileName));

	CAttribute movie1plot = CAttribute(plot);
	movie1plot.setStringValue("Luke Skywalker joins forces with a Jedi Knight, a cocky pilot,"
			" a wookiee and two droids to save the universe from the Empire\'s world-destroying "
			"battle-station, while also attempting to rescue Princess Leia from the evil "
			"Darth Vader.");
	movie1Attributes.insert(make_pair(plot.idAttributeType, movie1plot));

	CAttribute movie1contentRating = CAttribute(contentRating);
	movie1contentRating.setStringValue("PG");
	movie1Attributes.insert(make_pair(contentRating.idAttributeType, movie1contentRating));

	CAttribute movie1onlineRating = CAttribute(onlineRating);
	movie1onlineRating.setNumericValue(8.8f);
	movie1Attributes.insert(make_pair(onlineRating.idAttributeType, movie1onlineRating));

	db.AddAttributesForObject(movie1Obj, movie1Attributes);

	map<int, CAttribute> movie2Attributes;

	CAttribute movie2fileName = CAttribute(fileName);
	movie2fileName.setStringValue(movie2File);
	movie2Attributes.insert(make_pair(fileName.idAttributeType, movie2fileName));

	CAttribute movie2plot = CAttribute(plot);
	movie2plot.setStringValue("After the rebels have been brutally overpowered by the Empire"
			" on their newly established base, Luke Skywalker takes advanced Jedi training with "
			"Master Yoda, while his friends are pursued by Darth Vader as part of his plan "
			"to capture Luke.");
	movie2Attributes.insert(make_pair(plot.idAttributeType, movie2plot));

	CAttribute movie2contentRating = CAttribute(contentRating);
	movie2contentRating.setStringValue("PG");
	movie2Attributes.insert(make_pair(contentRating.idAttributeType, movie2contentRating));

	CAttribute movie2onlineRating = CAttribute(onlineRating);
	movie2onlineRating.setNumericValue(8.8f);
	movie2Attributes.insert(make_pair(onlineRating.idAttributeType, movie2onlineRating));

	db.AddAttributesForObject(movie2Obj, movie2Attributes);

	map<int, CAttribute> movie3Attributes;

	CAttribute movie3fileName = CAttribute(fileName);
	movie3fileName.setStringValue(movie3File);
	movie3Attributes.insert(make_pair(fileName.idAttributeType, movie3fileName));

	CAttribute movie3plot = CAttribute(plot);
	movie3plot.setStringValue("After rescuing Han Solo from the palace of Jabba the Hutt, "
			"the Rebels attempt to destroy the Second Death Star, while Luke Skywalker "
			"tries to bring his father back to the Light Side of the Force.");
	movie3Attributes.insert(make_pair(plot.idAttributeType, movie3plot));

	CAttribute movie3contentRating = CAttribute(contentRating);
	movie3contentRating.setStringValue("PG");
	movie3Attributes.insert(make_pair(contentRating.idAttributeType, movie3contentRating));

	CAttribute movie3onlineRating = CAttribute(onlineRating);
	movie3onlineRating.setNumericValue(8.4f);
	movie3Attributes.insert(make_pair(onlineRating.idAttributeType, movie3onlineRating));

	db.AddAttributesForObject(movie3Obj, movie3Attributes);

	int actionGenre = db.AddObject(GENRE, "action", "Action");
	int adventureGenre = db.AddObject(GENRE, "adventure", "Adventure");
	int scifiGenre = db.AddObject(GENRE, "scifi", "Science Fiction");

	int genreIds [3] = {actionGenre, adventureGenre, scifiGenre};

	int movieset1 = db.AddObject(MOVIESET, "starwars", "Star Wars Original Trilogy");
	int movieset2 = db.AddObject(MOVIESET, "starwars2", "Star Wars");

	int lukeActor = db.AddObject(ACTOR, "markhamil", "Mark Hamill");
	int hanActor = db.AddObject(ACTOR, "harford", "Harrison Ford");
	int leiaActor = db.AddObject(ACTOR, "fisher", "Carrie Fisher");

	int writer = db.AddObject(WRITER, "lucas", "George Lucas");

	int movieIds [3] = {movie1Obj, movie2Obj, movie3Obj	};

	for(int i = 0; i < 3; i++)
	{
		db.LinkObjectToObject(MOVIE_HAS_ACTOR, movieIds[i], lukeActor, "Luke Skywalker");
		db.LinkObjectToObject(MOVIE_HAS_ACTOR, movieIds[i], hanActor, "Han Solo");
		db.LinkObjectToObject(MOVIE_HAS_ACTOR, movieIds[i], leiaActor, "Princess Leia");

		db.LinkObjectToObject(MOVIE_HAS_WRITER, movieIds[i], writer);

		for(int j = 0; j < 3; j++)
		{
			db.LinkObjectToObject(MOVIE_HAS_GENRE, movieIds[i], genreIds[j]);
		}

		db.LinkObjectToObject(MOVIESET_HAS_MOVIE, movieset1, movieIds[i], "", i+1);
		db.LinkObjectToObject(MOVIESET_HAS_MOVIE, movieset2, movieIds[i], "", i+4);

	}


}

void TestObjects::InsertTestTvShow(CObjectDatabase& db)
{
	CStdString tvParent = "/home/zach/tvshows/";
	db.AddPath(tvParent);

	CStdString tvshowPath = tvParent + "The_Last_Airbender/";
	db.AddPath(tvshowPath);

	CStdString season1Path = tvshowPath + "Season_1/";
	CStdString season2Path = tvshowPath + "Season_2/";
	CStdString season3Path = tvshowPath + "Season_3/";

	db.AddPath(season1Path);
	db.AddPath(season2Path);
	db.AddPath(season3Path);

	CStdString episode1x01 = season1Path + "1x01.avi";
	CStdString episode1x02 = season1Path + "1x02.avi";
	CStdString episode2x03 = season2Path + "2x03.avi";
	CStdString episode2x04 = season2Path + "2x04.avi";
	CStdString episode3x05 = season3Path + "3x05.avi";
	CStdString episode3x06 = season3Path + "3x06.avi";

	int ep1Dir = db.AddDirEnt(episode1x01);
	int ep2Dir = db.AddDirEnt(episode1x02);
	int ep3Dir = db.AddDirEnt(episode2x03);
	int ep4Dir = db.AddDirEnt(episode2x04);
	int ep5Dir = db.AddDirEnt(episode3x05);
	int ep6Dir = db.AddDirEnt(episode3x06);

	int tvshowObj = db.AddObject(TVSHOW, "tla", "Avatar: The Last Airbender");
	int season1Obj = db.AddObject(SEASON, "tla1", "The Last Airbender: Season 1");
	int season2Obj = db.AddObject(SEASON, "tla2", "The Last Airbender: Season 2");
	int season3Obj = db.AddObject(SEASON, "tla3", "The Last Airbender: Season 3");

	int ep1Obj = db.AddObject(EPISODE, "tla1x01", "The Boy in the Iceberg");
	int ep2Obj = db.AddObject(EPISODE, "tla1x02", "The Avatar Returns");
	int ep3Obj = db.AddObject(EPISODE, "tla2x03", "Return to Omashu");
	int ep4Obj = db.AddObject(EPISODE, "tla2x04", "The Swamp");
	int ep5Obj = db.AddObject(EPISODE, "tla3x05", "The Beach");
	int ep6Obj = db.AddObject(EPISODE, "tla3x06", "The Avatar and the Firelord");

	int objIds [6] = {ep1Obj, ep2Obj, ep3Obj, ep4Obj, ep5Obj, ep6Obj};
	int dirIds [6] = {ep1Dir, ep2Dir, ep3Dir, ep4Dir, ep5Dir, ep6Dir};
	int seasonIds [3] {season1Obj, season2Obj, season3Obj};
	CStdString epFiles [6] = {episode1x01, episode1x02, episode2x03, episode2x04,
			episode3x05, episode3x06};

	for(int i = 0; i < 6; i++)
	{
		db.LinkObjectToDirent(objIds[i], dirIds[i]);
	}

	CAttributeType releaseDate;
	db.GetAttributeType(RELEASEDATE, releaseDate);
	CAttributeType filename;
	db.GetAttributeType(FILENAME, filename);
	CAttributeType summary;
	db.GetAttributeType(SUMMARY, summary);
	CAttributeType plot;
	db.GetAttributeType(EPISODE_PLOT, plot);

	map<int, CAttribute> show_attr;
	CAttribute released = CAttribute(releaseDate);
	released.setStringValue("2005");
	show_attr.insert(make_pair(releaseDate.idAttributeType, released));
	CAttribute showSummary = CAttribute(summary);
	showSummary.setStringValue("In a world where the four elements can be controlled by people "
			"known as Benders, the Fire Nation is waging a ruthless, oppressive war to control "
			"the other great nations. The only hope for stopping the brutal war led by the Firebenders "
			"rests on the shoulders of a reluctant young boy named Aang . The last known survivor "
			"of the peaceful Air Nomads and the Airbenders, Aang is the Avatar. "
			"The Avatar is the physical re-incarnation of the world spirit, with the power "
			"to manipulate all four elements and draw upon the power, knowledge, and experiences "
			"of the Avatar\'s previous incarnations. The Avatar is responsible for maintaining "
			"balance between the four nations of the world and the spirit world, hence keeping "
			"the balance of the world. Aided by a protective teenage Waterbender named Katara and her "
			"bull-headed brother Sokka, Aang begins a perilous journey to restore balance to "
			"their war-torn world");
	show_attr.insert(make_pair(summary.idAttributeType, showSummary));

	db.AddAttributesForObject(tvshowObj, show_attr);

	CStdString plot1 = "Two teenage siblings (Sokka and Katara) living in the South Pole"
			" discover a very strange boy (Aang), along with his pet flying bison Appa, "
			"in an iceberg. But can they trust him when he accidentally draws the war "
			"to their village\'s doorstep?";
	CStdString plot2 = "Katara and Sokka discover that Aang is the long-lost Avatar"
			" and must rescue him when a Firebending prince named Zuko kidnaps him.";
	CStdString plot3 = "Despite the shocking news the gang have just discovered, Aang"
			" insists on trying to find King Bumi within Omashu\'s borders, even with "
			"the dangers that will await him and his friends if his real identity is "
			"discovered.  Meanwhile Azula goes in search of old friends to help her "
			"with her mission.";
	CStdString plot4 = "A mysterious tornado pulls the kids into a supernatural swamp "
			"where Katara, Sokka and Aang have mysterious visions. Meanwhile, Momo and Appa "
			"are chased by rogue swamp dwellers and only Aang and the gang can save them.";
	CStdString plot5 = "Zuko, Azula, Mai and Ty Lee take a vacation to ember island."
			"In the process, they come to learn more about themselves and each other as well"
			" as their desires for the future. Elsewhere, Aang and others face off against a"
			" new adversary.";
	CStdString plot6 = "The tale of Avatar Roku and Fire Lord Sozin is revealed to "
			"Aang and Zuko by different sources. Both learn about themselves and are "
			"shown the origins of the war.";

	CStdString plots [6] = {plot1, plot2, plot3, plot4, plot5, plot6};
	CStdString dates [6] = {"2005-02-21", "2005-02-21", "2006-04-07", "2006-04-14", "2007-10-19",
			"2007-10-26"};

	for(int i = 0; i < 6; i++)
	{
		map<int, CAttribute> attr;

		CAttribute date = CAttribute(releaseDate);
		date.setStringValue(dates[i]);
		attr.insert(make_pair(releaseDate.idAttributeType, date));

		CAttribute epFile = CAttribute(filename);
		epFile.setStringValue(epFiles[i]);
		attr.insert(make_pair(filename.idAttributeType, epFile));

		CAttribute epPlot = CAttribute(plot);
		epPlot.setStringValue(plots[i]);
		attr.insert(make_pair(plot.idAttributeType, epPlot));

		db.AddAttributesForObject(objIds[i], attr);
	}

	int actionGenre = db.AddObject(GENRE, "action", "Action");
	int adventureGenre = db.AddObject(GENRE, "adventure", "Adventure");
	int scifiGenre = db.AddObject(GENRE, "scifi", "Science Fiction");

	int genreIds [3] = {actionGenre, adventureGenre, scifiGenre};
	CStdString links [3] = {"Book One: Water", "Book Two: Earth", "Book Three: Fire"};

	for(int i = 0; i < 3; i++)
	{
		db.LinkObjectToObject(TVSHOW_HAS_GENRE, tvshowObj, genreIds[i]);
		db.LinkObjectToObject(TVSHOW_HAS_SEASON, tvshowObj, seasonIds[i], links[i], i+1);
	}

	db.LinkObjectToObject(SEASON_HAS_EPISODE, season1Obj, ep1Obj, "", 1);
	db.LinkObjectToObject(SEASON_HAS_EPISODE, season1Obj, ep2Obj, "", 2);
	db.LinkObjectToObject(SEASON_HAS_EPISODE, season2Obj, ep3Obj, "", 3);
	db.LinkObjectToObject(SEASON_HAS_EPISODE, season2Obj, ep4Obj, "", 4);
	db.LinkObjectToObject(SEASON_HAS_EPISODE, season3Obj, ep5Obj, "", 5);
	db.LinkObjectToObject(SEASON_HAS_EPISODE, season3Obj, ep6Obj, "", 6);

	int studio = db.AddObject(STUDIO, "nick", "Nickelodeon");
	db.LinkObjectToObject(TVSHOW_HAS_STUDIO, tvshowObj, studio);

	int aang = db.AddObject(ACTOR, "zacheisen", "Zachary Tyler Eisen");
	int katara = db.AddObject(ACTOR, "maewhitman", "Mae Whitman");
	int sokka = db.AddObject(ACTOR, "jackdesena", "Jack DeSena");
	int chan = db.AddObject(ACTOR, "erikdetten", "Erik von Detten");

	db.LinkObjectToObject(TVSHOW_HAS_ACTOR, tvshowObj, aang, "Aang");
	db.LinkObjectToObject(TVSHOW_HAS_ACTOR, tvshowObj, katara, "Katara");
	db.LinkObjectToObject(TVSHOW_HAS_ACTOR, tvshowObj, sokka, "Sokka");

	db.LinkObjectToObject(EPISODE_HAS_ACTOR, ep5Obj, chan, "Chan");


}

void TestObjects::InsertTestAlbum(CObjectDatabase& db)
{

}



