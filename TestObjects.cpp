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

	int movieset = db.AddObject(MOVIESET, "starwars", "Star Wars Original Trilogy");

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

		db.LinkObjectToObject(MOVIESET_HAS_MOVIE, movieset, movieIds[i], "", i+1);


	}


}

void TestObjects::InsertTestTvShow(CObjectDatabase& db)
{

}

void TestObjects::InsertTestAlbum(CObjectDatabase& db)
{

}



