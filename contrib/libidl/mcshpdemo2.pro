@mcs_healplib   ; MCS-Base and MCS-HEALPix user contributed Functions and Pro

PRO mcshpdemo2, Host=host, User=user, Password=password, DBName=dbname, $
    Query=query, $
    TABName=tabname, Order=order, Nested=nested, $
    OutFile=outfile, Units=units, Gnomic=gnomic, Debug=debug, $
; xxxView passed options
    PNG=png, _EXTRA=e
;+
; NAME:
;       MCSHPDEMO2
;
; PURPOSE:
;   MCS-DIF-HEALPix demo program which shows how to obtain a map with the data
;   selected from any column of the DB table.
;   As usual the query can contain operations on one or more columns.
;
;   Show usage of DBExec_Qry and healp_MapFill, healp_MapSave.
;
; CATEGORY:
;   MCS-DIF demonstration program using the mcs_usrlib/mcs_healplib libraries.
;
; CALLING SEQUENCE:
;   mcshpdemo2 [, see keywords section]
;
; INPUT PARAMETERS:
;   None.
;
; INPUT KEYWORDS:
;      Host: The DB server host name (def. ross.iasfbo.inaf.it).
;      User: The user name to use for DB access (see code for def.).
;  Password: The password to use for DB access (see code for def.).
;    DBName: The database name to access (see code for def.).
;     Query: The complete query string (see code for def.).
;            Note that it has to produce a two columns output!
;   TABName: The reference table name (i.e. without the "_htm" or "_healp"
;            extensions) used to get auxiliary info like the map Order/Nside.
;            If not given then must give the Order keyword when the Query is
;            an input.
;     Order: The map Order (resolution level). Can give the TABName instead.
;    Nested: If set then assume NESTED scheme (used only if Order kwd passed).
;   OutFile: If set then save in that file (def. into "mcsdb_map.fits").
;     Units: Units of output data (used for the plot).
;            (def. string between AS and FROM in the query).
;    Gnomic: If set then plot Gnomic view (see gnomview help).
;     Debug: If set then print some debug info.
;       PNG: If set then save map into PNG files (see mollview help).
;
; Any extra keyword is passed to mollview / orthview / gnomview.
;
; NOTES:
;   To use the default query, the HEALPix indexed table "UCAC_2orig" must be
;   present in the selected database (can get it from ross.iasfbo.inaf.it/MCS/).
;   Here we assume tables are DIF indexed with HTM depth=6 and HEALPix order=8,
;   schema=NESTED.
;
;   Requires the HEALPix IDL library (see healpix.jpl.nasa.gov/).
;
;   It is assumed that the query output has 2 columns.
;   In case constant factors are used in the query, then it must be in
;   exponential notation (this is for MySQL - MCS communication reasons)!
;   For example: SELECT healpID, Mag1/1E2 FROM ....
;
; COMMON BLOCKS:
;   Note that all the info are passed using COMMON blocks and typically
;   in a main PRO need to include HEALPmap (see below).
;
;    HEALPmap, hp_nested, hp_nside, hp_order, hp_npix, $
;              hp_emptypix, hp_npixok, hp_map_nocc, hp_map_sum
;
;
; MODIFICATION HISTORY:
;   Written by L. Nicastro @ IASF-INAF, September 2007.
;  01/08/2008: LN: changed to use mcs_usrlib v0.3a.
;  03/10/2008: LN: changed to use the new mcs_usrlib/mcs_healplib v0.3b.
;  22/03/2009: LN: added NESTED kwd to mollview, orthview, gnomview call.
;  22/06/2010: LN: DIF v. 0.5.2 compatibility.
;  08/09/2010: LN: assume test catalogues were DIF indexed with HTM depth=6
;                  and HEALPix order=8, schema=NESTED.
;-

COMMON HEALPmap, hp_nested, hp_nside, hp_order, hp_npix, $
                 hp_emptypix, hp_npixok, hp_map_nocc, hp_map_sum

; Keywords section
  IF (NOT KEYWORD_SET(query)) THEN BEGIN
    tabname = 'UCAC_2orig'
    query = 'SELECT healpID_nest_8,Amm/1e3 as Amag FROM UCAC_2orig_healp_nest_8 WHERE DIF_Circle(0, 0, 1500)'
    PRINT, 'Query: ', query
  ENDIF

  IF (NOT KEYWORD_SET(order)) THEN BEGIN
    IF (NOT KEYWORD_SET(tabname)) THEN $
      MESSAGE, 'ERR: need to give Order or TABName keyword.'
  ENDIF ELSE tabname = 'UCAC_2orig'   ; Just for the plot title


; Open DB conn. and execute the query: return 0 if OK
  err = DBExec_Qry(dbname, user, password, query, Host=host)

print, 'Query executed'
  if (err) then begin
    DBClose
    MESSAGE, 'ERR: executing query.'
  endif

; Get/set Nside (need order or tabname in input)
; If order passed then can also pass the scheme
  IF (NOT KEYWORD_SET(order)) THEN $
    err = healp_MapNside(tabname) $
  ELSE BEGIN
    hp_order = order
    IF (KEYWORD_SET(nested)) THEN $
      hp_nested = 1 $
    ELSE $
      hp_nested = 0
  ENDELSE

  If (err EQ 0) THEN BEGIN
    err = healp_MapFill(/NORM, /INFO)
    IF (hp_npixok EQ 0) THEN $
      err = 1  ; Error
  ENDIF
  DBClose                          ; Free resources

  iF (err NE 0) THEN RETURN  ; Error?

; Units and plot title
  IF (NOT KEYWORD_SET(units)) THEN BEGIN
     qq = STRUPCASE(query)
     icom = STRPOS(qq, ',', /REVERSE_SEARCH) + 1
     ias = STRPOS(qq, ' AS ') + 4
     ii = MAX([icom,ias])
     ifrom = STRPOS(qq, 'FROM') -1
     units = STRMID(query, ii, ifrom-ii)
  ENDIF
  ptitle = 'MCS - DIF demo plot: '+ tabname

; Views
  IF (KEYWORD_SET(gnomic)) THEN BEGIN
    gnomview, hp_map_sum, /GRATICULE, COORD=['C','C'], $
              TITLE=ptitle, UNITS=units, NESTED=hp_nested, $
              PNG=png, _EXTRA=e
  ENDIF ELSE BEGIN
    mollview, hp_map_sum, /GRATICULE, COORD=['C','C'], $
              TITLE=ptitle, UNITS=units, NESTED=hp_nested, $
              PNG=png, _EXTRA=e
    orthview, hp_map_sum, /GRATICULE, COORD=['C','C'], ROT=[30,30], $
              TITLE=ptitle, UNITS=units, NESTED=hp_nested, $
              PNG=png, _EXTRA=e
  ENDELSE

PRINT, 'There are '+ STRTRIM(hp_npixok, 1) +' not empty pixels.'

; List of non empty pixels
;  idxs = where(hp_map_sum ne hp_emptypix,nfill)
;print, 'There are '+ strtrim(nfill,1) +' not empty pixels.'
;print, idxs
;print, hp_map_nocc[idxs]


  IF (KEYWORD_SET(outfile)) THEN BEGIN
    PRINT, 'Saving into file.'
    err = healp_MapSave(OUTFILE=outfile, UNITS=units, /SUM)
  ENDIF

END
