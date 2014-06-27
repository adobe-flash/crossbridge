program grading (input, output);

const
        namelength = 34;
          idlength = 12;
     commentlength =  6;
    headlinelength = 40;

type

    scores =
        (
         assign1, assign2, assign3, assign4,
         assign5, assign6, assign7, assign8, assigns,
         exam1, exam2, better, final, total
        );

    gradetype =
        (
               A, Aminus,
        Bplus, B, Bminus,
        Cplus, C, Cminus,
        Dplus, D, Dminus,
               F
        );

    grades =
        (
         absolu, curved, course
        );

        namestring = packed array [1..    namelength] of char;
          idstring = packed array [1..      idlength] of char;
     commentstring = packed array [1.. commentlength] of char;
    headlinestring = packed array [1..headlinelength] of char;

    studentpointer = ^ studentrecord;
    studentrecord =
        record
                   name : namestring;
                     id : idstring;
                  score : array [scores] of integer;
                  grade : array [grades] of gradetype;
                   rank : integer;
             percentile : integer;
                comment : commentstring;
                   next : studentpointer;
             nextinrank : studentpointer
        end;

var
    studentlist : studentpointer;
    nonames : boolean;
    nstudents, noshows, nofinals : integer;

    scoresfile : text;

    histogram : array [0..100] of integer;
    histogramlist : array [0..100] of studentpointer;

    scoretorank : array [0..100] of integer; (* scoretorank [76] = rank of student(s)
                                                with total score of 76 *)
    percent : array [0..100] of integer; (* percent [76] = percentile of student(s)
                                            with total score of 76 *)

function isnoshow (student : studentrecord) : boolean;
    begin
        isnoshow := (student.score [total] = 0)
    end (* isnoshow *);

procedure computescoretorank;
    var
        score, nhigher : integer;
    begin
        nhigher := 0;
        for score := 100 downto 0 do begin
            scoretorank [score] := 1 + nhigher;
            if
                (nstudents - noshows - nofinals) > 0
            then
                percent [score] :=
                    ((nstudents - noshows - nofinals - nhigher) * 100)
                    div
                    (nstudents - noshows - nofinals)
            else
               percent [score] := 0;
            nhigher := nhigher + histogram [score];
        end
    end (* computescoretorank *);

procedure settitle (headline : headlinestring);
    begin
        writeln ('.bp');
        writeln ('.ds Ti ', headline)
    end (* settitle *);

procedure writetroffheader;
    begin
        writeln ('.po 1.2c');
        writeln ('.m3 0');
        writeln ('.m4 10');
        writeln ('.ps 8');
        writeln ('.vs 10');
        writeln ('.pl 10.0i');
        writeln ('.ll 7.3i');
        writeln ('.lt 7.3i');
        writeln ('\ ');
        writeln ('.bp');
        writeln ('.de $f');
        writeln ('.ev 1');
        writeln ('.nf');
        writeln ('.ti 4.85c');
        writeln ('\fC\ Gr\ \ \ \ \ Id\ \ \ \ \ \ \ \ A1\ A2\ A3\ A4\ A5\ A6\ A7\ A8\ A\ \ E1\ E2\ E\ \ Fi\ To\ Ab\ Cu\ Gr\ Rank\ Percentile\ Coll');
        writeln ('.sp 2');
        writeln ('.in 1.0i');
        writeln ('\fC\s+2A  = \fRsum of all assignments');
        writeln ('\fCE1 = \fRfirst exam');
        writeln ('\fCE2 = \fRsecond exam');
        writeln ('\fCE  = \fR better of first two exams');
        writeln ('\fCFi = \fRfinal exam');
        writeln ('.sp -5');
        writeln ('.in +2.3i');
        writeln ('\fCTo = \fRtotal score in course');
        writeln ('\fCAb = \fRgrade according to first (absolute, i.e. not curved) policy');
        writeln ('\fCCu = \fRgrade according to second (curved) policy');
        writeln ('\fCGr = \fRcourse grade');
        writeln ('Percentiles are computed ignoring ''No shows'' and ''No finals''.\s-2');
        writeln ('.sp 2');
        writeln ('.in 0');
        writeln ('.tl ^\s+8\fB\\*(Ti^^\*(td\s-8\fP^^');
        writeln ('.ev');
        writeln ('..');
        writeln ('.de $h');
        writeln ('.ev 1');
        writeln ('.ps 8');
        writeln ('.vs 10');
        writeln ('\ ');
        writeln ('.sp |2.5c');
        writeln ('.ti 4.85c');
        writeln ('\fC\ Gr\ \ \ \ \ Id\ \ \ \ \ \ \ \ A1\ A2\ A3\ A4\ A5\ A6\ A7\ A8\ A\ \ E1\ E2\ E\ \ Fi\ To\ Ab\ Cu\ Gr\ Rank\ Percentile\ Coll');
        writeln ('.sp 2');
        writeln ('.ev');
        writeln ('..');
        writeln ('\ ');
        writeln ('.bp');
        writeln ('\fC');
        writeln ('.nf');
        writeln ('.ev 1');
        writeln ('.ps 8');
        writeln ('.vs 10');
        writeln ('.ev');
    end (* writetroffheader *);

procedure initialize;
    var
        score : integer;
    begin
        nstudents := 0;
        noshows := 0;
        nofinals := 0;
        studentlist := nil;

        for score := 0 to 100 do begin
            histogram [score] := 0;
            histogramlist [score] := nil
        end;

        writetroffheader
    end (* initialize *);

procedure readscores;
    var
        newstudent : studentpointer;
        s : scores;
        c : integer;
    begin
        new (newstudent);
        nstudents := nstudents + 1;

        with newstudent^ do begin
            next := studentlist;

            for c := 1 to namelength do
                read (input, name [c]);

            for c := 1 to idlength do
                read (input, id   [c]);

            for s := assign1 to assign8 do
                read (input, score [s]);

            read (input, score [exam1]);
            read (input, score [exam2]);
            read (input, score [final]);
            
            for c := 1 to commentlength do
                read (input, comment [c])
        end;

        readln (input);
        studentlist := newstudent
    end (* readscores *);

procedure computescores (student : studentpointer);
    var
        s : scores;
    begin
        with student^ do begin
            score [assigns] := 0;
            for s := assign1 to assign8 do
                score [assigns] := score [assigns] + score [s];

            if
                score [exam1] > score [exam2]
            then
                score [better] := score [exam1]
            else
                score [better] := score [exam2];

            score [total] :=    score [assigns]
                              + score [better ]
                              + score [final  ];

            if
                isnoshow (student^)
            then
                noshows := noshows + 1
            else if
                score [final] = 0
            then
                nofinals := nofinals + 1
            else
                histogram [score [total]] := histogram [score [total]] + 1;
      
        end
    end (* computescores *);

procedure computeallscores (studentlist : studentpointer);
    begin
        if
            studentlist <> nil
        then begin
            computescores (studentlist);
            computeallscores (studentlist^.next)
        end
    end (* computeallscores *);
    
procedure computegrades (student : studentpointer);
    
    begin
        with student^ do begin
            if score [total] >= 90 then grade [absolu] := A      else
            if score [total] >= 88 then grade [absolu] := Aminus else
            if score [total] >= 86 then grade [absolu] := Bplus  else
            if score [total] >= 80 then grade [absolu] := B      else
            if score [total] >= 78 then grade [absolu] := Bminus else
            if score [total] >= 76 then grade [absolu] := Cplus  else
            if score [total] >= 70 then grade [absolu] := C      else
            if score [total] >= 65 then grade [absolu] := Cminus else
            if score [total] >= 60 then grade [absolu] := Dplus  else
            if score [total] >= 55 then grade [absolu] := D      else
            if score [total] >= 50 then grade [absolu] := Dminus else
                                        grade [absolu] := F
            ;

                  rank := scoretorank   [score [total]];
            nextinrank := histogramlist [score [total]];
                          histogramlist [score [total]] := student;
            percentile := percent       [score [total]];

            if percentile >= 80 then grade [curved] := A      else
            if percentile >= 78 then grade [curved] := Aminus else
            if percentile >= 76 then grade [curved] := Bplus  else
            if percentile >= 50 then grade [curved] := B      else
            if percentile >= 48 then grade [curved] := Bminus else
            if percentile >= 46 then grade [curved] := Cplus  else
            if percentile >= 25 then grade [curved] := C      else
            if percentile >= 23 then grade [curved] := Cminus else
            if percentile >= 21 then grade [curved] := Dplus  else
            if percentile >= 10 then grade [curved] := D      else
            if percentile >=  8 then grade [curved] := Dminus else
                                     grade [curved] := F
            ;

           if
               grade [absolu] < grade [curved]
           then
               grade [course] := grade [absolu]
           else
               grade [course] := grade [curved]

        end
    end (* computegrades *);

procedure computeallgrades (studentlist : studentpointer);
    begin
        if
            studentlist <> nil
        then begin
            computegrades (studentlist);
            computeallgrades (studentlist^.next)
        end
    end (* computeallgrades *);

function gradepoint (g : gradetype) : real;
    begin
        if g = A      then gradepoint := 4.0 else
        if g = Aminus then gradepoint := 3.7 else
        if g = Bplus  then gradepoint := 3.3 else
        if g = B      then gradepoint := 3.0 else
        if g = Bminus then gradepoint := 2.7 else
        if g = Cplus  then gradepoint := 2.3 else
        if g = C      then gradepoint := 2.0 else
        if g = Cminus then gradepoint := 1.7 else
        if g = Dplus  then gradepoint := 1.3 else
        if g = D      then gradepoint := 1.0 else
        if g = Dminus then gradepoint := 0.7 else
        if g = F      then gradepoint := 0.0 else
                           gradepoint := 0.0
    end (* gradepoint *);

procedure writegrade (g : gradetype);
    begin
        if g = A      then write (' A ') else
        if g = Aminus then write (' A-') else
        if g = Bplus  then write (' B+') else
        if g = B      then write (' B ') else
        if g = Bminus then write (' B-') else
        if g = Cplus  then write (' C+') else
        if g = C      then write (' C ') else
        if g = Cminus then write (' C-') else
        if g = Dplus  then write (' D+') else
        if g = D      then write (' D ') else
        if g = Dminus then write (' D-') else
        if g = F      then write (' F ') else
                           write (' ??')
    end (* writegrade *);

procedure readallscores;
    begin
        while
            not eof (input)
        do
            readscores
    end (* readallscores *);

procedure writestudent (student : studentpointer);
    var
        c : integer;
        s : scores;
        g : grades;
    begin
    end (* writestudent *);

procedure writestraight (studentlist : studentpointer);
    begin
        if
            studentlist <> nil
        then begin
            writestraight (studentlist^.next);
            writestudent (studentlist);
        end
    end (* writestraight *);

procedure gotoXY (x, y : integer);
    begin
        writeln ('\ ');
        writeln ('.sp |', 1500 - 40 * y : 0, 'u');
        writeln ('.ti  ',        40 * x : 0, 'u');
    end (* gotoXY *);

procedure writehistogram;
    var
        score, height : integer;
    begin
        for score := 0 to 100 do
            for height := 1 to histogram [score] do begin
                gotoXY (score, height);
                writeln ('X')
            end;

        score := 0;
        repeat
            gotoXY (score, - 1);
            writeln ('\v''0.5c''|');
            gotoXY (score, - 3);
            writeln (score : 0);
            score := score + 5
        until
            score > 100;

        gotoXY (0,- 10)
    end (* writehistogram *);

procedure writerank (studentlist : studentpointer);
    begin
        if
            studentlist <> nil
        then begin
            writerank (studentlist^.nextinrank);
            writestudent (studentlist)
        end
    end (* writerank *);

procedure writebyrank;
    var
        score : integer;
    begin
        for score := 100 downto 0 do
            writerank (histogramlist [score])
    end (* writebyrank *);

procedure stats;
    var
        scoresum : array [scores] of real;
        gradecount : array [grades, gradetype] of integer;
        s   : scores;
        g   : grades;
        gt  : gradetype;
        currentstudent : studentpointer;
    begin
        writeln ('.in 0');
        writeln ('.hl');
        writeln ('\fC');
        writeln ('.2c');
        write   ('Number of students: ');
        writeln (nstudents : 5);
        write   ('          No-shows: ');
        writeln (noshows   : 5);
        write   ('         No finals: ');
        writeln (nofinals  : 5);
        write   ('      Assignment 1: ');
        writeln (scoresum [assign1] / (nstudents - noshows) : 5 : 1);
        write   ('      Assignment 2: ');
        writeln (scoresum [assign2] / (nstudents - noshows) : 5 : 1);
        write   ('      Assignment 3: ');
        writeln (scoresum [assign3] / (nstudents - noshows) : 5 : 1);
        write   ('      Assignment 4: ');
        writeln (scoresum [assign4] / (nstudents - noshows) : 5 : 1);
        write   ('      Assignment 5: ');
        writeln (scoresum [assign5] / (nstudents - noshows) : 5 : 1);
        write   ('      Assignment 6: ');
        writeln (scoresum [assign6] / (nstudents - noshows) : 5 : 1);
        write   ('      Assignment 7: ');
        writeln (scoresum [assign7] / (nstudents - noshows) : 5 : 1);
        write   ('      Assignment 8: ');
        writeln (scoresum [assign8] / (nstudents - noshows) : 5 : 1);
        write   ('   All assignments: ');
        writeln (scoresum [assigns] / (nstudents - noshows) : 5 : 1);
        write   ('            Exam 1: ');
        writeln (scoresum [exam1  ] / (nstudents - noshows) : 5 : 1);
        write   ('            Exam 2: ');
        writeln (scoresum [exam2  ] / (nstudents - noshows) : 5 : 1);
        write   ('     Better of 1,2: ');
        writeln (scoresum [better ] / (nstudents - noshows) : 5 : 1);
        write   ('        Final exam: ');
        writeln (scoresum [final  ] / (nstudents - noshows) : 5 : 1);
        write   ('       Total score: ');
        writeln (scoresum [total  ] / (nstudents - noshows) : 5 : 1);
        
        writeln ('.sp 2');
        writeln ('Absolute grade distribution: ');
        for gt := A to F do begin
            writegrade (gt);
            writeln (gradecount [absolu, gt])
        end;
        writeln ('.bc');

        writeln ('Curved grade distribution: ');
        for gt := A to F do begin
            writegrade (gt);
            writeln (gradecount [curved, gt])
        end;
        writeln ('.sp 2');

        writeln ('Course grade distribution: ');
        for gt := A to F do begin
            writegrade (gt);
            writeln (gradecount [course, gt])
        end;

    end (* stats *);

begin
    initialize;
    readallscores;
    computeallscores (studentlist);
    computescoretorank;
    computeallgrades (studentlist);

    settitle ('CSCI 1200, Spring 1989');
    nonames := false;
    writestraight (studentlist);

    settitle ('CSCI 1200, Spring 1989');
    nonames := true;
    writestraight (studentlist);

    settitle ('CSCI 1200, Spring 1989, grades by rank');
    nonames := false;
    writebyrank;

    settitle ('CSCI 1200, Spring 1989, grade statistics');
    writeln ('.de $f');
    writeln ('.tl ^\v''1.0i''\s+8\fB\\*(Ti^^\*(td\s-8\fP\v''-1.0i''^^');
    writeln ('..');
    writeln ('.de $h');
    writeln ('..');
    writeln ('\ ');
    writeln ('.bp');
    writeln ('Histogram, without ''No-shows'' and ''No finals''');
    writehistogram;
    stats;
end.
