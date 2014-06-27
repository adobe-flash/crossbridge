#
# cgc_forward -- counts forward references (update and reads)
# cgc_deref -- counts scans of the newspace area
# cgc_updates -- counts updates for forwarding pointer
#
# _cwrd counts words copied (transports and promotes)
# _pwrd counts promotes

function abs(x) {
	if (x < 0) return -x;
	else return x;
}

function update (idx) {
	unknown = $2
	fix = $3
	char = $4
	big = $5
	float = $6 + $7
	string = $8
	ivec = $9 + $10
	special = $11
	conscount = $12
	ratcomp = $13 + $14
	symbol = $15
	nil = $16
	gvec = $17 + $18 + $19
	totalcount = $20
	cons[idx] = conscount
	fixed[idx] = float + ratcomp + symbol + nil + unknown + fix + char
	variable[idx] = big + string + ivec + special + gvec
	total[idx] = totalcount;
}

function update_totals() {

	total_length += pause_length;

	for (idx in indices) {
		totalcons[idx] += cons[idx];
		totalfixed[idx] += fixed[idx];
		totalvariable[idx] += variable[idx];
		totaltotal[idx] += total[idx];
	}
	
	total_updates += updates;
	total_forwards += forwards;
	total_derefs += derefs;
}

function check_values () {

	for (idx in indices) {
		if (cons[idx] + fixed[idx] + variable[idx] != total[idx]) {
			printf "ERROR -- bad sum: %s, cons %d, fixed %d, variable %d, total %d\n", \
				idx, cons[idx], fixed[idx], variable[idx], total[idx]
			exit;
		}
		if (totalcons[idx] + totalfixed[idx] + totalvariable[idx] != totaltotal[idx]) {
			printf "ERROR -- bad total sum: %s, cons %d, fixed %d, variable %d, total %d\n", \
				idx, totalcons[idx], totalfixed[idx], totalvariable[idx], totaltotal[idx]
			exit;
		}
	}
}

function print_values () {

	for (idx in indices) {
		printf "CHECK: %s, cons %d, fixed %d, variable %d, total %d\n", \
			idx, cons[idx], fixed[idx], variable[idx], total[idx]
	}
	for (idx in indices) {
		printf "CHECK: totals: %s, cons %d, fixed %d, variable %d, total %d\n", \
			idx, totalcons[idx], totalfixed[idx], totalvariable[idx], totaltotal[idx]
	}
}

function initialize() {

	indices["cw"] = "cw"
	indices["co"] = "co"
	indices["aw"] = "aw"
	indices["ao"] = "ao"
	indices["pw"] = "pw"
	indices["po"] = "po"

	for (idx in indices) {
		totalfixed[idx] = 0;
		totalcons[idx] = 0;
		totalvariable[idx] = 0;
		totaltotal[idx] = 0;
	}

	total_length = 0;
	gccount = 0;
}

function calculate_costs (name, consarg, fixedarg, vararg, totalarg, derefarg, forwardarg, pause_length_arg) {

	# calculate the costs:

	all_objects = totalarg["co"];
	cons_objects = consarg["co"];
	fixed_objects = fixedarg["co"]
	var_objects = vararg["co"];

	real_forwards = (forwardarg - all_objects);
#print forwardarg, real_forwards, all_objects;
	cons_frac = (cons_objects / all_objects);
	fixed_frac = (fixed_objects / all_objects);
	var_frac = (var_objects / all_objects);

	# previously allocation = 6 instr per object allocated

	# calculate the costs:
	# allocation:
	#	4 instr per cons
	# 	11 instr per fixed (4 to get the vector and index)
	#	17 instr per variable (like fixed, but 6 to allocate relocatable part)

	alloc_cost = 2 * totalarg["ao"] + totalarg["aw"]

#print totalarg["ao"], alloc_cost

	a = derefarg;
	b = 0; # can't tell without count
	c = a - b;
	e = forwardarg;
	d = c - e;
        f = cons_objects + cons_frac * real_forwards;
	g = cons_objects;
	h = cons_frac * real_forwards;
#print cons_frac, real_forwards, h;
	i = consarg["co"] - consarg["po"];
	j = consarg["po"];
	e1 = g;
	k = e - f;
	l = fixed_objects + fixed_frac * real_forwards;
	m = fixed_frac * real_forwards;
	n = fixed_objects;
	o = fixedarg["co"] - fixedarg["po"];
	p = fixedarg["po"];
	e2 = n;
	q = k - l;
#	print q, (var_objects + var_frac * real_forwards)
	if ((abs(q) - abs(int((var_objects + var_frac * real_forwards)))) > 2) {
		print "var check error", q, (var_objects + var_frac * real_forwards)
		exit
	}
	r = var_frac * real_forwards;
	s = var_objects;
	t = vararg["co"] - vararg["po"];
	u = vararg["po"];
	e3 = s;
	e4 = e1 + e2 + e3;
	e5 = e4 + h + m + r;
	e6 = e5 + b + d;
	if (int(a) != int(e6)) {
		print "total check error", a, e6
		exit;
	}
	get_copy_cost = 5
	set_copy_cost = 3
	acost = 4
	bcost = 0
	ccost = 4
	dcost = 0
	ecost = 3
	fcost = 5
	gcost = get_copy_cost + 4
	hcost = 3
	icost = set_copy_cost + 2 + 3
	jcost = set_copy_cost + 4 + 3
	kcost = 3
	lcost = 5
	mcost = 3
	ncost = get_copy_cost + 4
	ocost = set_copy_cost + 2 
	pcost = set_copy_cost + 4 
	qcost = 5
	rcost = 3
	scost = get_copy_cost + 4
	tcost = set_copy_cost + 2
	ucost = set_copy_cost + 4
	e4cost = 4
	e5cost = 2
	e6cost = 4

#	print a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u;
	typeforwardcost = e * ecost + f * fcost + k * kcost + l * lcost + q * qcost;
	transportcost = g * gcost + h * hcost + i * icost + j * jcost + \
			m * mcost + n * ncost + o * ocost + p * pcost + \
			r * rcost + s * scost + t * tcost + u * ucost + \
			+ ((2 * fixedarg["cw"]) - fixedarg["co"]) + \
			+ ((4 * vararg["cw"]) - vararg["co"])
			

	scancost = a * acost + b * bcost + c * ccost + d * dcost + \
		   e6 * e6cost;
	updatecost = e4 * e4cost + e5 * e5cost;
	totalcost = alloc_cost + scancost + typeforwardcost + transportcost + updatecost;
	print name, gcstart, alloc_cost, scancost, typeforwardcost, transportcost, updatecost, totalcost, 6 * pause_length_arg;
}


BEGIN {

	initialize();

	total_forwards = 0
	total_derefs = 0;
	total_updates = 0;

}

$0 ~ /gc algorithm/ {
	algorithm = $4
}

$1 ~ /__gcbegin/ {
	togen = $2;
	gcstart = $4
	gccycle = 1;
}

(gccycle == 1) && $1 ~ /__pause_length/ {
	pause_length = $2;
}

(gccycle == 1) && $1 ~ /__cgc_forward/ {
	forwards = $2;
}

(gccycle == 1) && $1 ~ /__cgc_test/ {
	updates = $2;
}

(gccycle == 1) && $1 ~ /__cgc_deref/ {
	derefs = $2;
}

(gccycle == 1) && $1 ~ /__cwrd/ {
	update("cw");
}

(gccycle == 1) && $1 ~ /__cobj/ {
	update("co");
}

(gccycle == 1) && $1 ~ /__awrd/ {
	update("aw");
}

(gccycle == 1) && $1 ~ /__aobj/ {
	update("ao");
}

(gccycle == 1) && $1 ~ /__pobj/ {
	update("po");
}

(gccycle == 1) && $1 ~ /__pwrd/ {
	update("pw")

	gccount += 1;
	update_totals();
	check_values();
	calculate_costs("Current", cons, fixed, variable, total, derefs, forwards, pause_length);
}


END {
	check_values();
	calculate_costs("Total", totalcons, totalfixed, totalvariable, totaltotal, total_derefs, total_forwards, total_length);
}
