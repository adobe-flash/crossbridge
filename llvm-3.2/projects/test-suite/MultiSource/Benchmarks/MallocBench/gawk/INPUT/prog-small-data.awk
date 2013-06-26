# Memory configuration:
# word size 4, page size(words) 1024, pages per segment 20000
# gc semispaces(words) 5000000, 5000000, 5000000, 6000000
# gc threshold 128000 500000 1000000 796094581
# promote count 3 3 3 795046760
# stack simulation: top n 250, opoints 100
# file prefix: /tmp/zorn/e/results/slcomp128k
#
# Simulation configuration:
# test id: slcomp128k
# gc algorithm: stcp
# stack simulation on, cache tracing on, wide output on
# hash table size 500009
# availability: memory size 8000000, cache size 128000
# expected: urefs 3000000, refs 5000000, cycles 0
#
# Stack simulation:
# S1 start 40000000, size 30000000, warmstart 3000000, after gc 100000
# S2 start -1, size 50000, warmstart 0, after gc 100000
# Cache traces:
# S1 start 40000000, size 20000000
# S2 start -1, size 50000
#
__address_range_invalidation 317501 317501
__address_range_invalidation 643065 643065
__address_range_invalidation 643065 643065
__address_range_invalidation 738639 738639
__print_curr_data 738639 738639
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	3810	88	167	14640	4030	4475	0	88799	0	30	9838	1	2946	624	0	129448	
__awrd	0	0	0	15024	176	668	111910	959452	72366	0	177598	0	120	59028	6	65192	5496	0	1467036	
__cobj	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__cwrd	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__pobj	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__pwrd	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__remscans 0
__dirty_pages 126 0 0 58
__gcbegin 0 738639 738639
__gcends 802670 738639
__pause_length 64031
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	1	0	0	225	9	0	0	4437	0	0	220	0	256	0	0	5148	
__lvo1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	14	0	10	0	1130	0	0	402	1	14	3	0	1574	
__lvo4	0	0	0	1	0	0	239	9	10	0	5567	0	0	622	1	270	3	0	6722	
__lvs0	0	0	0	4	0	0	1104	1906	0	0	8874	0	0	1320	0	4740	0	0	17948	
__lvs1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	296	0	118	0	2260	0	0	2412	6	1608	30	0	6730	
__lvs4	0	0	0	4	0	0	1400	1906	118	0	11134	0	0	3732	6	6348	30	0	24678	
__cgc_scan 5155
__cgc_forward 7583
__cgc_deref 14242
__cgc_test 6310
__address_range_invalidation 1225900 1161869
__address_range_invalidation 1521247 1457216
__print_curr_data 1521247 1457216
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	4939	0	0	63	11	0	0	34852	0	0	21	0	2630	0	0	42516	
__awrd	0	0	0	19756	0	0	210	2552	0	0	69704	0	0	126	0	36010	0	0	128358	
__cobj	0	0	0	1	0	0	225	9	0	0	4437	0	0	220	0	256	0	0	5148	
__cwrd	0	0	0	4	0	0	1104	1906	0	0	8874	0	0	1320	0	4740	0	0	17948	
__pobj	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__pwrd	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__remscans 7
__dirty_pages 142 0 0 38
__gcbegin 0 1521247 1457216
__gcends 1582593 1457216
__pause_length 61346
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	23	11	0	0	2547	0	0	92	0	135	0	0	2808	
__lvo1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	2	0	2	0	426	0	0	186	1	4	0	0	621	
__lvo4	0	0	0	0	0	0	25	11	2	0	2973	0	0	278	1	139	0	0	3429	
__lvs0	0	0	0	0	0	0	90	2552	0	0	5094	0	0	552	0	2480	0	0	10768	
__lvs1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	132	0	20	0	852	0	0	1116	6	1092	0	0	3218	
__lvs4	0	0	0	0	0	0	222	2552	20	0	5946	0	0	1668	6	3572	0	0	13986	
__cgc_scan 4559
__cgc_forward 6938
__cgc_deref 12091
__cgc_test 5733
__address_range_invalidation 2008354 1882977
__address_range_invalidation 2279276 2153899
__print_curr_data 2279276 2153899
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	4341	0	0	120	15	0	0	34027	0	0	50	0	2690	0	0	41243	
__awrd	0	0	0	17352	0	0	408	2256	0	0	68054	0	0	300	0	39870	0	0	128240	
__cobj	0	0	0	1	0	0	248	20	0	0	3897	0	0	241	0	152	0	0	4559	
__cwrd	0	0	0	4	0	0	1194	4458	0	0	7794	0	0	1446	0	3396	0	0	18292	
__pobj	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__pwrd	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__remscans 0
__dirty_pages 129 0 0 50
__gcbegin 0 2279276 2153899
__gcends 2335784 2153899
__pause_length 56508
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	51	15	0	0	1379	0	0	50	0	22	0	0	1517	
__lvo1	0	0	0	0	0	0	0	0	0	0	220	0	0	0	0	1	0	0	221	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	4	0	7	0	634	0	0	255	1	8	0	0	909	
__lvo4	0	0	0	0	0	0	55	15	7	0	2233	0	0	305	1	31	0	0	2647	
__lvs0	0	0	0	0	0	0	328	2256	0	0	2758	0	0	300	0	534	0	0	6176	
__lvs1	0	0	0	0	0	0	0	0	0	0	440	0	0	0	0	10	0	0	450	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	268	0	74	0	1268	0	0	1530	6	1398	0	0	4544	
__lvs4	0	0	0	0	0	0	596	2256	74	0	4466	0	0	1830	6	1942	0	0	11170	
__cgc_scan 4331
__cgc_forward 4886
__cgc_deref 10151
__cgc_test 4227
__address_range_invalidation 2705982 2524097
__address_range_invalidation 2968599 2786714
__print_curr_data 2968599 2786714
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	2808	0	0	297	26	0	0	32692	0	0	135	0	2656	0	0	38614	
__awrd	0	0	0	11180	0	0	1028	1508	0	0	65384	0	0	810	0	48096	0	0	128006	
__cobj	0	0	0	1	0	0	296	35	0	0	3656	0	0	291	0	48	0	0	4327	
__cwrd	0	0	0	4	0	0	1386	6714	0	0	7312	0	0	1746	0	1460	0	0	18622	
__pobj	0	0	0	1	0	0	225	9	0	0	1486	0	0	220	0	19	0	0	1960	
__pwrd	0	0	0	4	0	0	1104	1906	0	0	2972	0	0	1320	0	936	0	0	8242	
__remscans 4
__dirty_pages 130 3 0 45
__gcbegin 0 2968599 2786714
__gcends 3026057 2786714
__pause_length 57458
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	135	26	0	0	2755	0	0	137	0	80	0	0	3133	
__lvo1	0	0	0	0	0	0	1	0	0	0	242	0	0	0	0	5	0	0	248	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	4	0	5	0	731	0	0	250	1	8	0	0	999	
__lvo4	0	0	0	0	0	0	140	26	5	0	3728	0	0	387	1	93	0	0	4380	
__lvs0	0	0	0	0	0	0	542	1508	0	0	5510	0	0	822	0	1350	0	0	9732	
__lvs1	0	0	0	0	0	0	130	0	0	0	484	0	0	0	0	86	0	0	700	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	268	0	50	0	1462	0	0	1500	6	1398	0	0	4684	
__lvs4	0	0	0	0	0	0	940	1508	50	0	7456	0	0	2322	6	2834	0	0	15116	
__cgc_scan 4915
__cgc_forward 5016
__cgc_deref 11018
__cgc_test 4284
__address_range_invalidation 3338083 3098740
__address_range_invalidation 3658911 3419568
__address_range_invalidation 3658911 3419568
__address_range_invalidation 3658911 3419568
__address_range_invalidation 3662814 3423471
__print_curr_data 3662814 3423471
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	2505	0	0	341	14	0	0	32109	0	0	137	0	2740	0	0	37846	
__awrd	0	0	0	9956	0	0	1150	1282	0	0	64218	0	0	822	0	50974	0	0	128402	
__cobj	0	0	0	0	0	0	206	52	0	0	4344	0	0	206	0	106	0	0	4914	
__cwrd	0	0	0	0	0	0	824	6316	0	0	8688	0	0	1236	0	1506	0	0	18570	
__pobj	0	0	0	0	0	0	21	11	0	0	816	0	0	21	0	11	0	0	880	
__pwrd	0	0	0	0	0	0	84	2552	0	0	1632	0	0	126	0	66	0	0	4460	
__remscans 1
__dirty_pages 131 4 0 64
__gcbegin 0 3662814 3423471
__gcends 3738724 3423471
__pause_length 75910
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	1	0	0	139	14	0	0	3193	0	0	137	0	35	0	0	3519	
__lvo1	0	0	0	0	0	0	1	0	0	0	250	0	0	2	0	6	0	0	259	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	8	0	25	0	2105	0	0	318	1	19	1	0	2477	
__lvo4	0	0	0	1	0	0	148	14	25	0	5548	0	0	457	1	60	1	0	6255	
__lvs0	0	0	0	4	0	0	546	1282	0	0	6386	0	0	822	0	2560	0	0	11600	
__lvs1	0	0	0	0	0	0	130	0	0	0	500	0	0	12	0	152	0	0	794	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	414	0	316	0	4210	0	0	1908	6	2336	10	0	9200	
__lvs4	0	0	0	4	0	0	1090	1282	316	0	11096	0	0	2742	6	5048	10	0	21594	
__cgc_scan 6564
__cgc_forward 8149
__cgc_deref 16134
__cgc_test 6399
__address_range_invalidation 4195984 3880731
__address_range_invalidation 4429854 4114601
__print_curr_data 4429854 4114601
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	4108	0	0	881	8	0	0	35038	0	0	319	0	2247	0	0	42601	
__awrd	0	0	0	16158	0	0	2954	2052	0	0	70076	0	0	1914	0	34850	0	0	128004	
__cobj	0	0	0	1	0	0	324	55	0	0	5770	0	0	322	0	76	0	0	6548	
__cwrd	0	0	0	4	0	0	1286	5046	0	0	11540	0	0	1932	0	2806	0	0	22614	
__pobj	0	0	0	0	0	0	50	15	0	0	825	0	0	50	0	15	0	0	955	
__pwrd	0	0	0	0	0	0	198	2256	0	0	1650	0	0	300	0	90	0	0	4494	
__remscans 16
__dirty_pages 129 5 0 56
__gcbegin 0 4429854 4114601
__gcends 4511181 4114601
__pause_length 81327
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	307	8	0	0	2043	0	0	314	0	48	0	0	2720	
__lvo1	0	0	0	0	0	0	1	0	0	0	388	0	0	9	0	5	0	0	403	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	6	0	26	0	1457	0	0	440	1	8	0	0	1938	
__lvo4	0	0	0	0	0	0	314	8	26	0	3888	0	0	763	1	61	0	0	5061	
__lvs0	0	0	0	0	0	0	1256	2052	0	0	4086	0	0	1884	0	1958	0	0	11236	
__lvs1	0	0	0	0	0	0	130	0	0	0	776	0	0	54	0	86	0	0	1046	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	272	0	308	0	2914	0	0	2640	6	1398	0	0	7538	
__lvs4	0	0	0	0	0	0	1658	2052	308	0	7776	0	0	4578	6	3442	0	0	19820	
__cgc_scan 6795
__cgc_forward 8502
__cgc_deref 16868
__cgc_test 6481
__address_range_invalidation 4912535 4515955
__address_range_invalidation 5166600 4770020
__print_curr_data 5166600 4770020
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	2620	0	0	465	19	0	0	32406	0	0	186	0	2619	0	0	38315	
__awrd	0	0	0	10416	0	0	1590	1400	0	0	64812	0	0	1116	0	48672	0	0	128006	
__cobj	0	0	0	1	0	0	579	48	0	0	5485	0	0	579	0	89	0	0	6781	
__cwrd	0	0	0	4	0	0	2338	4842	0	0	10970	0	0	3474	0	3110	0	0	24738	
__pobj	0	0	0	0	0	0	135	26	0	0	1941	0	0	135	0	26	0	0	2263	
__pwrd	0	0	0	0	0	0	542	1508	0	0	3882	0	0	810	0	156	0	0	6898	
__remscans 14
__dirty_pages 132 3 0 48
__gcbegin 0 5166600 4770020
__gcends 5238994 4770020
__pause_length 72394
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	173	19	0	0	2460	0	0	183	0	22	0	0	2857	
__lvo1	0	0	0	0	0	0	1	0	0	0	558	0	0	7	0	5	0	0	571	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	5	0	13	0	1276	0	0	419	1	10	0	0	1724	
__lvo4	0	0	0	0	0	0	179	19	13	0	4294	0	0	609	1	37	0	0	5152	
__lvs0	0	0	0	0	0	0	742	1400	0	0	4920	0	0	1098	0	1188	0	0	9348	
__lvs1	0	0	0	0	0	0	130	0	0	0	1116	0	0	42	0	86	0	0	1374	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	270	0	130	0	2552	0	0	2514	6	1474	0	0	6946	
__lvs4	0	0	0	0	0	0	1142	1400	130	0	8588	0	0	3654	6	2748	0	0	17668	
__cgc_scan 5794
__cgc_forward 7162
__cgc_deref 14634
__cgc_test 5374
__address_range_invalidation 5598445 5129471
__address_range_invalidation 5883700 5414726
__address_range_invalidation 5883700 5414726
__print_curr_data 5883700 5414726
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	2514	0	0	595	22	0	0	33436	0	0	234	0	2394	0	0	39195	
__awrd	0	0	0	9958	0	0	1938	1270	0	0	66872	0	0	1404	0	46562	0	0	128004	
__cobj	0	0	0	1	0	0	617	41	0	0	4472	0	0	616	0	46	0	0	5793	
__cwrd	0	0	0	4	0	0	2538	4734	0	0	8944	0	0	3696	0	2696	0	0	22612	
__pobj	0	0	0	1	0	0	137	14	0	0	1786	0	0	137	0	15	0	0	2090	
__pwrd	0	0	0	4	0	0	540	1282	0	0	3572	0	0	822	0	996	0	0	7216	
__remscans 1
__dirty_pages 131 5 0 55
__gcbegin 0 5883700 5414726
__gcends 5979846 5414726
__pause_length 96146
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	213	22	0	0	5618	0	0	220	0	217	0	0	6290	
__lvo1	0	0	0	0	0	0	1	0	0	0	859	0	0	21	0	5	0	0	886	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	6	0	23	0	1559	0	0	491	1	10	0	0	2090	
__lvo4	0	0	0	0	0	0	220	22	23	0	8036	0	0	732	1	232	0	0	9266	
__lvs0	0	0	0	0	0	0	842	1270	0	0	11236	0	0	1320	0	4132	0	0	18800	
__lvs1	0	0	0	0	0	0	130	0	0	0	1718	0	0	126	0	86	0	0	2060	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	276	0	258	0	3118	0	0	2946	6	1474	0	0	8078	
__lvs4	0	0	0	0	0	0	1248	1270	258	0	16072	0	0	4392	6	5692	0	0	28938	
__cgc_scan 8346
__cgc_forward 9128
__cgc_deref 21098
__cgc_test 6352
__address_range_invalidation 6354224 5789104
__address_range_invalidation 6585395 6020275
__print_curr_data 6585395 6020275
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	3661	0	0	572	17	0	0	32364	0	0	216	0	2263	0	0	39093	
__awrd	0	0	0	14498	0	0	1952	1890	0	0	64728	0	0	1296	0	43640	0	0	128004	
__cobj	0	0	0	0	0	0	692	49	0	0	6657	0	0	692	0	245	0	0	8335	
__cwrd	0	0	0	0	0	0	2838	4722	0	0	13314	0	0	4152	0	4758	0	0	29784	
__pobj	0	0	0	0	0	0	307	8	0	0	609	0	0	307	0	9	0	0	1240	
__pwrd	0	0	0	0	0	0	1256	2052	0	0	1218	0	0	1842	0	512	0	0	6880	
__remscans 11
__dirty_pages 140 3 0 49
__gcbegin 0 6585395 6020275
__gcends 6662430 6020275
__pause_length 77035
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	212	17	0	0	3519	0	0	220	0	23	0	0	3991	
__lvo1	0	0	0	0	0	0	1	0	0	0	1044	0	0	12	0	5	0	0	1062	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	5	0	16	0	1356	0	0	421	1	10	0	0	1809	
__lvo4	0	0	0	0	0	0	218	17	16	0	5919	0	0	653	1	38	0	0	6862	
__lvs0	0	0	0	0	0	0	864	1890	0	0	7038	0	0	1320	0	3196	0	0	14308	
__lvs1	0	0	0	0	0	0	130	0	0	0	2088	0	0	72	0	86	0	0	2376	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	270	0	182	0	2712	0	0	2526	6	1474	0	0	7170	
__lvs4	0	0	0	0	0	0	1264	1890	182	0	11838	0	0	3918	6	4756	0	0	23854	
__cgc_scan 6089
__cgc_forward 8069
__cgc_deref 15840
__cgc_test 6062
__address_range_invalidation 7093954 6451799
__address_range_invalidation 7312170 6670015
__print_curr_data 7312170 6670015
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	2389	0	0	471	15	2	0	33501	0	0	174	0	2477	0	0	39029	
__awrd	0	0	0	9494	0	0	1842	1318	20	0	67002	0	0	1044	0	47298	0	0	128018	
__cobj	0	0	0	0	0	0	588	58	0	0	4790	0	0	587	0	64	0	0	6087	
__cwrd	0	0	0	0	0	0	2428	4560	0	0	9580	0	0	3522	0	3442	0	0	23532	
__pobj	0	0	0	0	0	0	172	19	0	0	678	0	0	172	0	19	0	0	1060	
__pwrd	0	0	0	0	0	0	740	1400	0	0	1356	0	0	1032	0	114	0	0	4642	
__remscans 2
__dirty_pages 131 5 0 58
__gcbegin 0 7312170 6670015
__gcends 7377361 6670015
__pause_length 65191
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	1	0	0	171	15	1	0	2734	0	0	175	0	22	0	0	3119	
__lvo1	0	0	0	0	0	0	1	0	0	0	1155	0	0	12	0	7	0	0	1175	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	9	0	26	0	1176	0	0	351	1	17	0	0	1580	
__lvo4	0	0	0	1	0	0	181	15	27	0	5065	0	0	538	1	46	0	0	5874	
__lvs0	0	0	0	4	0	0	684	1318	10	0	5468	0	0	1050	0	1966	0	0	10500	
__lvs1	0	0	0	0	0	0	130	0	0	0	2310	0	0	72	0	1064	0	0	3576	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	416	0	300	0	2352	0	0	2106	6	1820	0	0	7000	
__lvs4	0	0	0	4	0	0	1230	1318	310	0	10130	0	0	3228	6	4850	0	0	21076	
__cgc_scan 5215
__cgc_forward 6582
__cgc_deref 12874
__cgc_test 4947
__address_range_invalidation 7762010 7054664
__address_range_invalidation 8012252 7304906
__print_curr_data 8012252 7304906
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	2515	0	0	416	13	2	0	35033	0	0	151	0	2280	0	0	40410	
__awrd	0	0	0	9994	0	0	1642	1528	20	0	70066	0	0	906	0	43854	0	0	128010	
__cobj	0	0	0	1	0	0	587	54	1	0	3919	0	0	582	0	61	0	0	5205	
__cwrd	0	0	0	4	0	0	2372	4478	10	0	7838	0	0	3492	0	2200	0	0	20394	
__pobj	0	0	0	0	0	0	204	22	0	0	840	0	0	204	0	22	0	0	1292	
__pwrd	0	0	0	0	0	0	824	1270	0	0	1680	0	0	1224	0	132	0	0	5130	
__remscans 10
__dirty_pages 130 4 0 54
__gcbegin 0 8012252 7304906
__gcends 8103962 7304906
__pause_length 91710
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	1	0	0	155	13	2	0	5273	0	0	155	0	155	0	0	5754	
__lvo1	0	0	0	0	0	0	1	0	0	0	1268	0	0	20	0	7	0	0	1296	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	8	0	21	0	1300	0	0	471	1	16	0	0	1817	
__lvo4	0	0	0	1	0	0	164	13	23	0	7841	0	0	646	1	178	0	0	8867	
__lvs0	0	0	0	4	0	0	622	1528	20	0	10546	0	0	930	0	4556	0	0	18206	
__lvs1	0	0	0	0	0	0	130	0	0	0	2536	0	0	120	0	1064	0	0	3850	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	410	0	246	0	2600	0	0	2826	6	1816	0	0	7904	
__lvs4	0	0	0	4	0	0	1162	1528	266	0	15682	0	0	3876	6	7436	0	0	29960	
__cgc_scan 7562
__cgc_forward 9505
__cgc_deref 19669
__cgc_test 7564
__address_range_invalidation 8422994 7623938
__address_range_invalidation 8713285 7914229
__print_curr_data 8713285 7914229
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	3235	0	0	452	14	0	0	33168	0	0	168	0	2438	0	0	39475	
__awrd	0	0	0	12834	0	0	1790	1494	0	0	66336	0	0	1008	0	44550	0	0	128012	
__cobj	0	0	0	1	0	0	538	45	2	0	6256	0	0	527	0	189	0	0	7558	
__cwrd	0	0	0	4	0	0	2170	4736	20	0	12512	0	0	3162	0	4882	0	0	27486	
__pobj	0	0	0	0	0	0	212	17	0	0	667	0	0	211	0	17	0	0	1124	
__pwrd	0	0	0	0	0	0	864	1890	0	0	1334	0	0	1266	0	102	0	0	5456	
__remscans 4
__dirty_pages 135 5 0 46
__gcbegin 0 8713285 7914229
__gcends 8784574 7914229
__pause_length 71289
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	1	0	0	161	15	0	0	3779	0	0	168	0	70	0	0	4194	
__lvo1	0	0	0	0	0	0	1	0	0	0	1490	0	0	18	0	7	0	0	1516	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	5	0	15	0	1291	0	0	416	1	15	0	0	1743	
__lvo4	0	0	0	1	0	0	167	15	15	0	6560	0	0	602	1	92	0	0	7453	
__lvs0	0	0	0	4	0	0	656	1756	0	0	7558	0	0	1008	0	3310	0	0	14292	
__lvs1	0	0	0	0	0	0	130	0	0	0	2980	0	0	108	0	1064	0	0	4282	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	270	0	150	0	2582	0	0	2496	6	1794	0	0	7298	
__lvs4	0	0	0	4	0	0	1056	1756	150	0	13120	0	0	3612	6	6168	0	0	25872	
__cgc_scan 5897
__cgc_forward 6453
__cgc_deref 15383
__cgc_test 4717
__address_range_invalidation 9270347 8400002
__address_range_invalidation 9381740 8511395
__print_curr_data 9381740 8511395
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	2919	0	0	457	12	0	0	34267	0	0	165	0	2249	0	0	40069	
__awrd	0	0	0	11570	0	0	1810	1412	0	0	68534	0	0	990	0	43694	0	0	128010	
__cobj	0	0	0	2	0	0	486	42	2	0	4784	0	0	476	0	101	0	0	5893	
__cwrd	0	0	0	8	0	0	1960	4340	20	0	9568	0	0	2856	0	3616	0	0	22368	
__pobj	0	0	0	0	0	0	171	15	1	0	741	0	0	167	0	17	0	0	1112	
__pwrd	0	0	0	0	0	0	684	1318	10	0	1482	0	0	1002	0	224	0	0	4720	
__remscans 4
__dirty_pages 133 3 0 45
__gcbegin 0 9381740 8511395
__gcends 9462210 8511395
__pause_length 80470
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	155	13	0	0	4633	0	0	160	0	89	0	0	5050	
__lvo1	0	0	0	0	0	0	1	0	0	0	1722	0	0	10	0	5	0	0	1738	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	5	0	14	0	1214	0	0	400	1	8	0	0	1642	
__lvo4	0	0	0	0	0	0	161	13	14	0	7569	0	0	570	1	102	0	0	8430	
__lvs0	0	0	0	0	0	0	624	1522	0	0	9266	0	0	960	0	3842	0	0	16214	
__lvs1	0	0	0	0	0	0	130	0	0	0	3444	0	0	60	0	86	0	0	3720	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	270	0	140	0	2428	0	0	2400	6	1398	0	0	6642	
__lvs4	0	0	0	0	0	0	1024	1522	140	0	15138	0	0	3420	6	5326	0	0	26576	
__cgc_scan 6592
__cgc_forward 8571
__cgc_deref 17101
__cgc_test 6526
__address_range_invalidation 10944638 9993823
__print_curr_data 10944638 9993823
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	109	0	0	8	0	0	0	30186	0	0	4	0	3905	0	0	34212	
__awrd	0	0	0	434	0	0	30	0	0	0	60372	0	0	24	0	67150	0	0	128010	
__cobj	0	0	0	1	0	0	470	39	1	0	5498	0	0	464	0	117	0	0	6590	
__cwrd	0	0	0	4	0	0	1900	4434	10	0	10996	0	0	2784	0	4008	0	0	24136	
__pobj	0	0	0	1	0	0	154	13	1	0	559	0	0	148	0	14	0	0	890	
__pwrd	0	0	0	4	0	0	620	1528	10	0	1118	0	0	888	0	82	0	0	4250	
__remscans 2
__dirty_pages 131 4 0 41
__gcbegin 0 10944638 9993823
__gcends 11227272 9993823
__pause_length 282634
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	4	1	0	0	19530	0	0	5	0	1809	0	0	21349	
__lvo1	0	0	0	0	0	0	1	0	0	0	1875	0	0	3	0	5	0	0	1884	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	4	0	6	0	291	0	0	185	1	8	0	0	495	
__lvo4	0	0	0	0	0	0	9	1	6	0	21696	0	0	193	1	1822	0	0	23728	
__lvs0	0	0	0	0	0	0	14	242	0	0	39060	0	0	30	0	30254	0	0	69600	
__lvs1	0	0	0	0	0	0	130	0	0	0	3750	0	0	18	0	86	0	0	3984	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	268	0	60	0	582	0	0	1110	6	1398	0	0	3424	
__lvs4	0	0	0	0	0	0	412	242	60	0	43392	0	0	1158	6	31738	0	0	77008	
__cgc_scan 22817
__cgc_forward 32818
__cgc_deref 68982
__cgc_test 26522
__address_range_invalidation 12200042 10966593
__print_curr_data 12200042 10966593
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	0	0	0	1236	0	0	0	54387	0	0	412	0	6	0	0	56041	
__awrd	0	0	0	0	0	0	4944	0	0	0	108774	0	0	2472	0	11814	0	0	128004	
__cobj	0	0	0	0	0	0	320	26	0	0	20313	0	0	320	0	1835	0	0	22814	
__cwrd	0	0	0	0	0	0	1294	2906	0	0	40626	0	0	1920	0	30410	0	0	77156	
__pobj	0	0	0	0	0	0	161	14	0	0	526	0	0	161	0	14	0	0	876	
__pwrd	0	0	0	0	0	0	656	1494	0	0	1052	0	0	966	0	84	0	0	4252	
__remscans 3
__dirty_pages 172 1 0 11
__gcbegin 0 12200042 10966593
__gcends 12725313 10966593
__pause_length 525271
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	0	0	0	412	0	0	0	36250	0	0	412	0	1803	0	0	38877	
__lvo1	0	0	0	0	0	0	0	0	0	0	2036	0	0	0	0	4	0	0	2040	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	2	0	0	0	617	0	0	216	1	4	0	0	840	
__lvo4	0	0	0	0	0	0	414	0	0	0	38903	0	0	628	1	1811	0	0	41757	
__lvs0	0	0	0	0	0	0	1648	0	0	0	72500	0	0	2472	0	29814	0	0	106434	
__lvs1	0	0	0	0	0	0	0	0	0	0	4072	0	0	0	0	64	0	0	4136	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	132	0	0	0	1234	0	0	1296	6	1092	0	0	3760	
__lvs4	0	0	0	0	0	0	1780	0	0	0	77806	0	0	3768	6	30970	0	0	114330	
__cgc_scan 51598
__cgc_forward 66944
__cgc_deref 127195
__cgc_test 60932
__address_range_invalidation 13164735 11406015
__address_range_invalidation 13330236 11571516
__print_curr_data 13330236 11571516
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	11116	0	0	699	1	0	0	20343	0	0	233	0	2898	0	0	35290	
__awrd	0	0	0	43798	0	0	2796	6210	0	0	40686	0	0	1398	0	33142	0	0	128030	
__cobj	0	0	0	0	0	0	571	12	0	0	48622	0	0	571	0	1821	0	0	51597	
__cwrd	0	0	0	0	0	0	2286	1412	0	0	97244	0	0	3426	0	30732	0	0	135100	
__pobj	0	0	0	0	0	0	155	12	0	0	471	0	0	155	0	12	0	0	805	
__pwrd	0	0	0	0	0	0	624	1412	0	0	942	0	0	930	0	72	0	0	3980	
__remscans 1
__dirty_pages 141 4 0 48
__gcbegin 1 71337878 60443088
__gcends 72788342 60443088
__pause_length 1450464
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__lvo0	0	0	0	1	0	0	46	3	0	0	5346	0	0	42	0	4	0	0	5442	
__lvo1	0	0	0	0	0	0	2	0	0	0	3964	0	0	10	0	7	0	0	3983	
__lvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvo3	0	0	0	0	0	0	7	0	14	0	992	0	0	406	1	16	0	0	1436	
__lvo4	0	0	0	1	0	0	55	3	14	0	10302	0	0	458	1	27	0	0	10861	
__lvs0	0	0	0	4	0	0	218	7494	0	0	10692	0	0	252	0	4112	0	0	22772	
__lvs1	0	0	0	0	0	0	134	0	0	0	7928	0	0	60	0	1064	0	0	9186	
__lvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__lvs3	0	0	0	0	0	0	404	0	148	0	1984	0	0	2436	6	1816	0	0	6794	
__lvs4	0	0	0	4	0	0	756	7494	148	0	20604	0	0	2748	6	6992	0	0	38752	
__cgc_scan 2191
__cgc_forward 2201
__cgc_deref 4781
__cgc_test 1212
__address_range_invalidation 105735040 85664874
__STCP_GCTOTALS
__print_curr_data 105735040 85664874
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__aobj	0	0	0	24795	0	0	2	0	0	0	798	0	0	1	0	2	0	0	25598	
__awrd	0	0	0	49594	0	0	8	0	0	0	1596	0	0	6	0	44	0	0	51248	
__cobj	0	0	0	0	0	0	0	0	0	0	2058	0	0	0	0	0	0	0	2058	
__cwrd	0	0	0	0	0	0	0	0	0	0	4116	0	0	0	0	0	0	0	4116	
__pobj	0	0	0	0	0	0	0	0	0	0	1323	0	0	0	0	0	0	0	1323	
__pwrd	0	0	0	0	0	0	0	0	0	0	2646	0	0	0	0	0	0	0	2646	
__remscans 133
__dirty_pages 51 258 0 122
__print_total_data 105735040 85664874
__NAMES	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__AOBJ	0	0	0	774728	93	167	65105	4859	4609	0	4263003	0	30	28450	1	236650	624	0	5378319	
__AWRD	0	0	0	2196274	186	668	314870	1127558	73764	0	8526006	0	120	170700	6	4478402	5496	0	16894050	
__COBJ	0	0	0	73	7	0	64785	3106	266	0	1560024	0	0	63478	0	53219	0	0	1744958	
__CWRD	0	0	0	272	14	0	273384	602676	2744	0	3120048	0	0	380868	0	1362540	0	0	5742546	
__POBJ	0	0	0	10	2	0	17300	827	66	0	191171	0	0	16956	0	8456	0	0	234788	
__PWRD	0	0	0	40	4	0	73058	167206	674	0	382342	0	0	101736	0	149460	0	0	874520	
__REMSCANS 2731
__LOOKUPS 3449995
__MISSES 134340
__total_dirty_bits 17118 1950 0 5502
__old_loc_hash_table 105735040 85664874
<GHT size 255007, entries 0, refs 0, hits 0, misses 0, 
 inserts 0, deletes 0, max_entries 0, collisions 0, lchain 0>
 >> full 1030, empty 253977, longest 2, entries 2058
__cgc_scan 0
__cgc_forward 0
__cgc_deref 0
__cgc_test 0
__cgc_total_scan 1747689
__cgc_total_forward 2118188
__cgc_total_deref 4709273
__cgc_total_test 1757613
__id_hash_table 105735040 85664874
<OHT size 500009, entries 0, refs 0, hits 0, misses 0, 
 inserts 0, deletes 0, max_entries -1, collisions 0, lchain 0>
 >> full 202098, empty 297911, longest 7, entries 257475
__loc_hash_table 105735040 85664874
<OHT size 500009, entries 0, refs 0, hits 0, misses 0, 
 inserts 0, deletes 0, max_entries -1, collisions 0, lchain 0>
 >> full 203421, empty 296588, longest 6, entries 257475
__big_obj_hash_table 105735040 85664874
<GHT size 1013, entries 0, refs 0, hits 0, misses 0, 
 inserts 0, deletes 0, max_entries 0, collisions 0, lchain 0>
 >> full 673, empty 340, longest 5, entries 1077
__store_contents_misses 355
__write_barrier_traps 5886
__vsize_indirect_refs 0
__total_refs 105735040
__total_urefs 85664874
__gc_overhead 20070166
__total_loads 497760
__total_loadps 63792050
__total_stores 967753
__total_storeps 4852879
__total_storeis 15554432
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__stit	0	0	0	2196020	10	0	205362	170012	1426	0	8415034	0	0	112998	0	4453570	0	0	15554432	
__stt0	0	0	0	0	0	0	0	0	0	0	993672	0	0	17839	0	2125182	0	0	3136693	
__stt1	0	0	0	0	0	0	0	0	0	0	8393	0	0	3619	0	144356	0	0	156368	
__stt2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__stt3	0	0	0	0	0	0	0	0	0	0	2102	0	0	1521267	0	36091	3	0	1559463	
__stt4	0	0	0	0	0	0	0	0	0	0	1004167	0	0	1542725	0	2305629	3	0	4852524	
__stc0	0	555378	0	0	0	0	51	3	919	0	1678351	0	0	64451	340993	496509	38	0	3136693	
__stc1	0	146487	0	0	0	0	0	0	30	0	4410	0	0	1974	3155	312	0	0	156368	
__stc2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__stc3	0	726838	0	0	0	0	0	640	0	0	2645	0	0	235619	592694	1023	4	0	1559463	
__stc4	0	1428703	0	0	0	0	51	643	949	0	1685406	0	0	302044	936842	497844	42	0	4852524	
__storep_target_contents_number 0 0 2127457
__storep_target_contents_number 0 1 31074
__storep_target_contents_number 0 2 0
__storep_target_contents_number 0 3 422784
__storep_target_contents_number 0 4 555378
__storep_target_contents_number 1 0 2389
__storep_target_contents_number 1 1 2427
__storep_target_contents_number 1 2 0
__storep_target_contents_number 1 3 5065
__storep_target_contents_number 1 4 146487
__storep_target_contents_number 2 0 0
__storep_target_contents_number 2 1 0
__storep_target_contents_number 2 2 0
__storep_target_contents_number 2 3 0
__storep_target_contents_number 2 4 0
__storep_target_contents_number 3 0 1880
__storep_target_contents_number 3 1 1617
__storep_target_contents_number 3 2 0
__storep_target_contents_number 3 3 829128
__storep_target_contents_number 3 4 726838
__storep_target_contents_number 4 0 0
__storep_target_contents_number 4 1 0
__storep_target_contents_number 4 2 0
__storep_target_contents_number 4 3 0
__storep_target_contents_number 4 4 0
__names	unknown	fixnum	char	bignum	sfloat	lfloat	string	ivec	code	special	cons	ratio	complex	symbol	nil	gvec	struct	user	total	
__nvo0	0	0	0	0	0	0	0	0	0	0	1	0	0	0	0	0	0	0	1	
__nvo1	0	0	0	3	1	0	17248	749	65	0	127973	0	0	16948	0	6540	0	0	169527	
__nvo2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__nvo3	0	0	0	0	0	0	392	0	191	0	6280	0	0	979	1	30	5	0	7878	
__nvo4	0	0	0	3	1	0	17640	749	256	0	134254	0	0	17927	1	6570	5	0	177406	
__nvs0	0	0	0	0	0	0	0	0	0	0	2	0	0	0	0	0	0	0	2	
__nvs1	0	0	0	12	2	0	72188	144262	650	0	255946	0	0	101688	0	110788	0	0	685536	
__nvs2	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	
__nvs3	0	0	0	0	0	0	2012	0	2920	0	12560	0	0	5874	6	3090	46	0	26508	
__nvs4	0	0	0	12	2	0	74200	144262	3570	0	268508	0	0	107562	6	113878	46	0	712046	
__ii_notes 129065
__ii_allocs 5249254
__ii_loads 64289810
__ii_loadmisses 14215
__ii_stores 5820632
__ii_storemisses 25759
__ii_internal_literals 134340
__ii_stored_literals 1428703
__ii_copy_counts 6824103
__ii_copy_misses 545
__ii_unknown_objects 0
__ii_bad_tag_refs 3027369
__ii_eq_tests 0
__ii_deaths 0
__ii_death_misses 0
