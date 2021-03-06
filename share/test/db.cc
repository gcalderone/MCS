// ----------------------------------------------------------------------^
// Copyright (C) 2004, 2005, 2006, 2007, 2008 Giorgio Calderone
// (mailto: <gcalderone@ifc.inaf.it>)
// 
// This file is part of MCS.
// 
// MCS is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// MCS is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with MCS; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
// 
// ----------------------------------------------------------------------$


//MCS include file
#include <mcs.hh>
using namespace mcs;


//See test.h for auxiliary functions and macros
#include "test.h"



int main(int argc, char* argv[])
{
  try {
    string s;
    int i, j, k;
    float f;
    void* buf;
    unsigned int size;
    Data* myd;
    Data* myd2;

    //A DBConn object is used to connect to the database server.
    DBConn db;

    //!A query object is used to execute queries and retrieve result (recordset).
    Query* qry;


    /*
    {
      Data dt(TIME);
      dt = 80;
	
      string s = dt.sval();
      struct tm ts;
      dt.tval(&ts);
      cout << asctime(&ts) << endl;

      time_t tt = dt.tval();
      cout << tt << " " << ctime(&tt) << endl;

      cout << time(NULL) << endl;
      dt.settimenow();
      dt.tval(&ts);
      cout << asctime(&ts) << endl;

      tt = dt.tval();
      cout << tt << " " << ctime(&tt) << endl;


      dt = "2005-02-01 00:00:00";
      dt.tval(&ts);
      cout << asctime(&ts) << endl;


      dt = "2005-08-01 00:00:00";
      dt.tval(&ts);
      cout << asctime(&ts) << endl;



      tt = dt.tval();
      cout << tt << " " << ctime(&tt) << endl;

      dt.tval(&ts);
      ts.tm_gmtoff *= 3;
      ts.tm_zone = NULL;

      dt.settimeval(ts);

      dt.tval(&ts);
      cout << asctime(&ts) << endl;
      tt = dt.tval();
      cout << tt << " " << ctime(&tt) << endl;

      tt = 0;
      gmtime_r(&tt, &ts);



      dt.settimenow();
      dt.tval(&ts);

      struct tm temp;
      temp = ts;

      tt = timegm(&ts);
      tt -= temp.tm_gmtoff;
      if (temp.tm_isdst == 1)
	  tt += 3600;
	
      localtime_r(&tt, &ts);



      tt = 0;
      gmtime_r(&tt, &ts);
      ts.tm_gmtoff = 7200;
      tt = timelocal(&ts);

      cout << tt << " " << ctime(&tt) << endl;
      cout << tt << " " << ctime(&tt) << endl;
    }
    */


    //Connect to database
    db.connect(TEST_USER, TEST_PASS, TEST_DBNA, "localhost");

    //Create a Query object
    qry = new Query(&db);

    //The simpleQuery() method can be used when you're not interested
    //in the recordset returned by the query or when the query doesn't
    //returns record at all.

    //Drop a table
    qry->simpleQuery("DROP TABLE IF EXISTS " + string(TEST_TBL) + "_empty");

    //Create a table
    s = "CREATE TABLE " TEST_TBL "_empty (i INTEGER)";
    qry->simpleQuery(s);

    //Drop a table
    qry->simpleQuery("DROP TABLE IF EXISTS " + string(TEST_TBL));

    //Create a table
    s = "CREATE TABLE " TEST_TBL
      " (c CHAR(10), i INTEGER, f FLOAT, d DATETIME, m MEDIUMINT, "
      " id INTEGER UNSIGNED AUTO_INCREMENT, ts TIMESTAMP, PRIMARY KEY(id))";
    qry->simpleQuery(s);

    //Prepare an INSERT query, the second parameter (a string)
    //contains all field that you want to valorize, the third
    //parameter is the table name.
    //qry->prepare_with_parameters(MCS_PAB_INSERT, "c i f d m", TEST_TBL);
    qry->prepare_with_parameters(MCS_PAB_INSERT, "*", TEST_TBL);

    //Check number of parameters is correct
    if (qry->param().count() != 7)
      ThrowErr("Wrong number of parameters (should be 7");

    //The prepare_with_parameters() is useful because you can set
    //values for the insert query using the Data class interface.
    qry->param()["c"] = "hello  ";
    qry->param()["i"] = 1;
    qry->param()["f"] = 1.23;
    qry->param()["d"] = 0;  //1 january 1970, 00:00:00 UTC
    qry->param()["m"] = 99;
    qry->param()["ts"] = "2005-12-01 00:00:00"; //Local time zone


    //Finally execute the insert query.
    qry->execute();	


    //Insert record 2
    qry->param()["c"] = "bye";
    qry->param()["i"] = 2;
    qry->param()["f"] = 4.56;
    //qry->param()["d"].settimenow(); //Computer time
    qry->param()["d"] = "2000-01-02 03:04:05";
    qry->param()["m"] = 1000000000; //Exceeds the MEDIUMINT capacity
    qry->param()["ts"].setNull();
    qry->execute();	


    //Insert record 3
    qry->param()[0] = "mmhhh";
    qry->param()[1] = 3;
    qry->param()[2] = 10.0E-3;

    //It is possible to set a TIME value in a Data object using a "struct tm"
    struct tm ts;

    //For example to set it to current datetime:
    time_t tt = time(NULL);
    localtime_r(&tt, &ts);

    //It is possible to set each field of the structure
    ts.tm_sec  = 1;  //seconds (0 - 59)
    ts.tm_min  = 2;  //minutes (0 - 59)
    ts.tm_hour = 3;  //hours   (0 - 11)
    ts.tm_mday = 4;  //day     (1 - 31)
    ts.tm_mon  = 1;  //month (0: Jan, 11: Dec)
    ts.tm_year = 2006 - 1900; //Year - 1900
    ts.tm_gmtoff = 3600; //number of seconds that must be added to UTC to get
			 //local time, excluding any adjustment for daylight
			 //saving.
    ts.tm_isdst = 0;   //flag (0, 1) indicating if DST is in action at the
		       //time specified.

    qry->param()[3].settimeval(ts);

    qry->param()[4].setNull();
    qry->execute();	


    //Insert record 4
    qry->param()[0] = "AHO";
    qry->param()[1] = 4;
    qry->param()[2] = -1.2345;
    //qry->param()[3] = "  1976-04-23   00:00:00 ";
    qry->param()[3] = "1976-04-23   00:00:00 ";
    //qry->param()[4].setNull();
    qry->execute();	


    //Insert record 5
    qry->param()["c"] = "---";
    qry->param()["i"] = 5;
    qry->param()["f"].setNull();
    //qry->param()["d"] = " 2006-02-14 11:12:13";
    qry->param()["d"] = "2006-02-14 11:12:13";
    //qry->param()["m"].setNull();
    qry->execute();	


    //Retrieving data from a table, prepare the query.
    qry->prepare("SELECT * FROM " TEST_TBL);

    //Execute the query, the parameter is used to tell if the entire
    //recordset should be stored on client side (like in this case).
    qry->execute(true);

    //Check number of fields and rows.
    if (qry->nFields() != 7)
      ThrowErr("Wrong number of fields: " + itos(qry->nFields()));

    if (qry->nRows() != 5)
      ThrowErr("Wrong number of records: " + itos(qry->nRows()));

    //Scan the entire recordset.
    while (! qry->eof()) {
      i = qry->pos();

      //Check i-th record contents.
      s = qry->rec()["c"].sval();
      j = qry->rec()["i"].ival();
      f = qry->rec()["f"].fval();
      Data& myd = qry->rec()["d"];
      //myd = new Data(qry->rec()["d"]);
      k = qry->rec()["m"].ival();
      Data& myd2 = qry->rec()["ts"];
      cout << s <<": "<< myd2.sval() << endl;
      cout << "tval(): "<< myd.tval() << endl;

      switch (i) {
      case 0:
	if (s != "hello")               ThrowErr("|" + s + "|");
	if (j != 1)                     ThrowErr(itos(j));
	if (! compareFloat(f, 1.23))    ThrowErr(dtos(f));
	if (myd.sval() != "1970-01-01 00:00:00") ThrowErr(myd.sval());
	if (k != 99)                    ThrowErr(itos(k));
	if (myd2.sval() != "2005-12-01 00:00:00")
	  ThrowErr(myd2.sval());
	break;
      case 1:
	if (s != "bye")                 ThrowErr("|" + s + "|");
	if (j != 2)                     ThrowErr(itos(j));
	if (! compareFloat(f, 4.56))    ThrowErr(dtos(f));
	if (myd.sval() != "2000-01-02 03:04:05")
	  ThrowErr(myd.sval());
	if (k != 8388607)               ThrowErr(itos(k));
	break;		
      case 2:
	if (s != "mmhhh")               ThrowErr("|" + s + "|");
	if (j != 3)                     ThrowErr(itos(j));
	if (! compareFloat(f, 0.01))    ThrowErr(dtos(f));
	if (myd.sval() != "2006-02-04 03:02:01")
	  ThrowErr(myd.sval());
	break;
      case 3:
	if (s != "AHO")                 ThrowErr("|" + s + "|");
	if (j != 4)                     ThrowErr(itos(j));
	if (! compareFloat(f, -1.2345)) ThrowErr(dtos(f));
	myd.tval(&ts);
	if (ts.tm_mday != 23  ||
	    ts.tm_mon  != 03  ||
	    ts.tm_year != 76)
	  ThrowErr(myd.sval());
	break;
      case 4:
	if (s != "---")                 ThrowErr("|" + s + "|");
	if (j != 5)                     ThrowErr(itos(j));
	if (! qry->rec()[2].isNull())   ThrowErr(dtos(f));
	myd.tval(&ts);
	if (ts.tm_sec  != 13  ||
	    ts.tm_min  != 12  ||
	    ts.tm_hour != 11)
	  ThrowErr(myd.sval());
	break;
      }

      //Test Record serialization procedure
      buf = qry->rec().getEntireBuffer(size);

      //Restore the record in a new object...
      Record rec(buf);

      //And compare the Data objects in the two record objects.
      for (j=0; j<qry->nFields(); j++)
	compareData(qry->rec()[j], rec[j]);

      //Advance the cursor in the recordset.
      qry->setNext();
    }


    //Update a record, this is similar to insert a new record: field's
    //value are stored in Data objects. Note that the fourth parameter
    //is the where clause that you would use in a SQL query.
    qry->prepare_with_parameters(MCS_PAB_UPDATE, "i c f", TEST_TBL, "id = 4");

    //Update values
    qry->param()[0] = 40;
    cout << qry->param()[0].sval() << "|" << endl;
    cout << qry->param()[1].sval() << "|" << endl;
    cout << qry->param()[2].sval() << "|" << endl;
    qry->param()[1] = qry->param()[1].sval() + "UPD";
    qry->param()[2] = qry->param()[2].fval() * (-1.0);

    //Execute the query.
    qry->execute();

    //Insert a new record and valorize only two fields.
    qry->prepare_with_parameters(MCS_PAB_INSERT, "i c", "mcstest");
    qry->param()[0] = 6;
    qry->param()[1] = "Giorgio";

    //Execute the query, the newly inserted record will have id=6.
    qry->execute();


    //Execute again the SELECT query.
    qry->prepare("SELECT * FROM " TEST_TBL);
    qry->execute(true);

    if (qry->nRows() != 6)
      ThrowErr("Wrong number of records: " + itos(qry->nRows()));

    //Move the recordset cursor to the fourth record (id=4).
    qry->setPos(3); //Note that the position start from 0.
    i = qry->pos();

    //The foruth record has been updated, so check record's content.
    s = qry->rec()[0].sval();
    j = qry->rec()[1].ival();
    f = qry->rec()[2].fval();
    if (s != "AHOUPD")              ThrowErr("|" + s + "|");
    if (j != 40)                    ThrowErr(itos(j));
    if (! compareFloat(f,  1.2345)) ThrowErr(dtos(f));

    //Move the cursor to the sixth-record, the last inserted.
    qry->setPos(5);
    i = qry->pos();

    //Check contents.
    s = qry->rec()[0].sval();
    j = qry->rec()[1].ival();
    if (s != "Giorgio")             ThrowErr("|" + s + "|");
    if (j != 6)                     ThrowErr(itos(j));
    if (! qry->rec()[2].isNull())   ThrowErr(dtos(f));
    if (! qry->rec()[3].isNull())   ThrowErr(qry->rec()[3].sval());


    //It is possible to read and/or modify a single table using a
    //Table object, which is a representation of the underlying
    //database table.

    //Create a Table object above the database table.
    Table tbl(&db, TEST_TBL, "id");

    //Move the cursor to the last record.
    tbl.setLast();

    //Update some field's value.
    tbl.setFieldMap("i c ");  //Note that the active record doesn't have field names
    //tbl.rec().setFieldMap( tbl.metarec() );
    tbl.rec()[0] = 60;
    tbl.rec()[1] = tbl.rec()[1].sval() + "UPD";

    //Update the record in the database table.
    tbl.update();

    //Assign value to the fields of a "dummy" record.
    tbl.newRec()[0] = "INSERT";
    tbl.newRec()[1] = 7;

    //And insert that record into the database table.
    tbl.insert();


    //Execte the SELECT query.
    qry->prepare("SELECT * FROM " TEST_TBL);
    qry->execute(true);

    if (qry->nRows() != 7)
      ThrowErr("Wrong number of records: " + itos(qry->nRows()));

    qry->setPos(5);
    i = qry->pos();


    //Check contents of the updated and inserted records.
    s = qry->rec()[0].sval();
    j = qry->rec()[1].ival();
    if (s != "GiorgioUPD")          ThrowErr("|" + s + "|");
    if (j != 60)                    ThrowErr(itos(j));
    if (! qry->rec()[2].isNull())   ThrowErr(dtos(f));
    if (! qry->rec()[3].isNull())   ThrowErr(qry->rec()[3].sval());

    qry->setPos(6);
    i = qry->pos();
    s = qry->rec()[0].sval();
    j = qry->rec()[1].ival();
    if (s != "INSERT")              ThrowErr("|" + s + "|");
    if (j != 7)                     ThrowErr(itos(j));
    if (! qry->rec()[2].isNull())   ThrowErr(dtos(f));
    if (! qry->rec()[3].isNull())   ThrowErr(qry->rec()[3].sval());


    //Create a new table with a blob-type field.
    qry->simpleQuery("DROP TABLE IF EXISTS " TEST_TBL "_blob");
    qry->simpleQuery("CREATE TABLE " TEST_TBL "_blob (b BLOB)");

    //Set up a binary structure.
    struct blob blob;
    blob.ii = 11;
    blob.ff = 3.1415;
    strcpy(blob.cc, "DBBLOBTest");

    //Prepare an insert query.
    qry->prepare_with_parameters(MCS_PAB_INSERT, "b", TEST_TBL "_blob");

    //Assign the binary structure to the Data object.
    qry->param()[0].setblob(&blob, sizeof(struct blob));

    //Insert the query.
    qry->execute();


    //Retrieve the record just written
    qry->prepare("SELECT * FROM " TEST_TBL "_blob");
    qry->execute(true);

    //Check number of fields and rows
    if (qry->nFields() != 1)
      ThrowErr("Wrong number of fields: " + itos(qry->nFields()));

    if (qry->nRows() != 1)
      ThrowErr("Wrong number of records: " + itos(qry->nRows()));

    //Get the pointer to the binary structure read from the table.
    struct DBBLOBTest* pblob = (struct DBBLOBTest*) qry->rec()[0].buffer();

    //Compare it to the written one.
    if (memcmp(pblob, &blob, sizeof(struct blob)))
      ThrowErr("BLOB object differs");


    //Select again from the table to print on standard output.
    qry->prepare("SELECT * FROM " TEST_TBL);
    qry->execute(true);

    //Print header, names
    for (i=0; i<qry->nFields(); i++)
      cout << qry->rec()[i].name() << "\t";
    cout << endl;

    //Print header, types
    for (i=0; i<qry->nFields(); i++)
      cout << Types2Str(qry->rec()[i].type(),
			qry->rec()[i].isUnsigned()) << "\t";
    cout << endl;

    //Print data
    while (! qry->eof()) {

      //Stress the Record serialization procedures.
      buf = qry->rec().getEntireBuffer(size);
      Record rec(buf);

      for (i=0; i<rec.count(); i++) {
	if (rec[i].isNull()) {
	  cout << "(NULL)\t";
	}
	else {
	  if (rec[i].type() == TIME)
	    cout << rec[i].tval();
	  else if (rec[i].type() == BLOB)
	    cout << hexDump(rec[i].buffer(), rec[i].maxLength());
	  else
	    cout << rec[i].sval();
	  cout << "\t";
	}
      }
      cout << endl;
      qry->setNext();
    }


    //Finally destroy the Query object and close the database
    //connection.
    delete qry;
    db.close();
  }
  catch (Event e) {
    cerr << e.where() << ": " << e.msg() << endl;
    exit(2);
  }
  catch (...) {
    cerr << "Unknown error: " << endl;
    exit(3);
  }

  exit(0);
}
