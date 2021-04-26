/*****************************************************
             PROJECT  : numaprof
             VERSION  : 1.1.2
             DATE     : 11/2020
             AUTHOR   : Valat Sébastien - CERN
             LICENSE  : CeCILL-C
*****************************************************/

/*******************  FUNCTION  *********************/
function setupNumaPageStats(data)
{
	//reformat data
	var formattedData = [{
		key: "Numa pages",
		values: []
    }];
	
	for (var i in data)
		formattedData[0].values.push({x:i,y:data[i]});

	//plot
	nv.addGraph(function() {
		var chart = nv.models.multiBarChart()
			//.transitionDuration(350)
			//.reduceXTicks(true)   //If 'false', every single x-axis tick label will be rendered.
			//.rotateLabels(0)      //Angle to rotate x-axis labels.
			.showControls(false)   //Allow user to switch between 'Grouped' and 'Stacked' mode.
			//.groupSpacing(0.1)    //Distance between each group of bars.
		;

		// 	chart.xAxis
		// 		.tickFormat(d3.format(',f'));
		// 
		// 	chart.yAxis
		// 		.tickFormat(d3.format(',.1f'));

		d3.select('#peakNumaPages svg')
			.datum(formattedData)
			.call(chart);

		nv.utils.windowResize(chart.update);

		return chart;
	});
}

/*******************  FUNCTION  *********************/
function setupDistanceCnt(data)
{
	//reformat data
	var formattedData = [{
		key: "Distance statistiques",
		values: []
    }];
	
	for (var i in data)
		formattedData[0].values.push({x:i-1,y:data[i]});

	//plot
	nv.addGraph(function() {
		var chart = nv.models.multiBarChart()
			//.transitionDuration(350)
			//.reduceXTicks(true)   //If 'false', every single x-axis tick label will be rendered.
			//.rotateLabels(0)      //Angle to rotate x-axis labels.
			.showControls(false)   //Allow user to switch between 'Grouped' and 'Stacked' mode.
			//.groupSpacing(0.1)    //Distance between each group of bars.
		;

		// 	chart.xAxis
		// 		.tickFormat(d3.format(',f'));
		// 
		// 	chart.yAxis
		// 		.tickFormat(d3.format(',.1f'));

		d3.select('#distanceCnt svg')
			.datum(formattedData)
			.call(chart);

		nv.utils.windowResize(chart.update);

		return chart;
	});
}

/*******************  FUNCTION  *********************/
function setupPieChart(divName,data)
{
	//Donut chart example
	nv.addGraph(function() {
		var chart = nv.models.pieChart()
			.x(function(d) { return d.label })
			.y(function(d) { return d.value })
			.showLabels(true)     //Display pie labels
			.labelThreshold(.05)  //Configure the minimum slice size for labels to show up
			.labelType("percent") //Configure what type of data to show in the label. Can be "key", "value" or "percent"
			.donut(true)          //Turn on Donut mode. Makes pie chart look tasty!
			.donutRatio(0.35)     //Configure how big you want the donut hole size to be.
			;

			d3.select("#"+divName+" svg")
				.datum(data)
				.transition().duration(350)
				.call(chart);

		nv.utils.windowResize(function(){ chart.update(); });
		
		return chart;
	});
}

/*******************  FUNCTION  *********************/
function setupHeadMap(svgId,data)
{
	var nodes = 0;
	for (var i in data)
		nodes++;
	
	var itemSize = 44,
		cellSize = itemSize - 1,
		margin = {top: 50, right: 20, bottom: 20, left: 90};
		
	var width = 50+50*nodes - margin.right - margin.left,
		height = 50+50*nodes - margin.top - margin.bottom;

	//convert
	var fdata = [];
	for (var i in data)
		for (var j in data[i])
			if (i == -1)
				fdata.push({source:"Unpinned",dest:j,value:data[i][j]});
			else
				fdata.push({source:i,dest:j,value:data[i][j]});

	var x_elements = d3.set(fdata.map(function( item ) { return item.dest; } )).values(),
		y_elements = d3.set(fdata.map(function( item ) { return item.source; } )).values();

	var xScale = d3.scale.ordinal()
		.domain(x_elements)
		.rangeBands([0, x_elements.length * itemSize]);

	var xAxis = d3.svg.axis()
		.scale(xScale)
		.tickFormat(function (d) {
			return d;
		})
		.orient("top");

	var yScale = d3.scale.ordinal()
		.domain(y_elements)
		.rangeBands([0, y_elements.length * itemSize]);

	var yAxis = d3.svg.axis()
		.scale(yScale)
		.tickFormat(function (d) {
			return d;
		})
		.orient("left");

	var colors = ["#6363FF", "#6373FF", "#63A3FF", "#63E3FF", "#63FFFB", "#63FFCB",
			"#63FF9B", "#63FF6B", "#7BFF63", "#BBFF63", "#DBFF63", "#FBFF63", 
			"#FFD363", "#FFB363", "#FF8363", "#FF7363", "#FF6364"];
	var heatmapColor = d3.scale.linear()
		.domain([d3.min(fdata, function(d) { return d.value; }), d3.max(fdata, function(d) { return d.value; })])
		.range(["#6363FF",  "#FF6364"]);
		//.range(colors);
		
	/* Initialize tooltip */
	var tip = d3.tip().attr('class', 'd3-tip').html(function(d) { return d.value; });

	var svg = d3.select('#accessMatrix')
		.append("svg")
		.attr("width", width + margin.left + margin.right)
		.attr("height", height + margin.top + margin.bottom)
		.append("g")
		.attr("transform", "translate(" + margin.left + "," + margin.top + ")");
		
	svg.call(tip);

	var cells = svg.selectAll('rect')
		.data(fdata)
		.enter().append('g').append('rect')
		.attr('class', 'cell')
		.attr('width', cellSize)
		.attr('height', cellSize)
		.attr('y', function(d) { return yScale(d.source); })
		.attr('x', function(d) { return xScale(d.dest); })
		.attr('fill', function(d) { return d.value == 0 ? "#CCCCCC" : heatmapColor(d.value); })
		.on('mouseover',tip.show)
		.on('mouseout',tip.hige);

	svg.append("g")
		.attr("class", "y axis")
		.call(yAxis)
		.selectAll('text')
		.attr('font-weight', 'normal');

	svg.append("g")
		.attr("class", "x axis")
		.call(xAxis)
		.selectAll('text')
		.attr('font-weight', 'normal')
		.style("text-anchor", "start")
		.attr("dx", ".8em")
		.attr("dy", ".5em")
		.attr("transform", function (d) {
			return "rotate(-65)";
		});
	
	// text label for the x axis
	svg.append("text")             
		.attr("transform",
				"translate(" + (width/2) + " ," + 
							( -25) + ")")
		.style("text-anchor", "middle")
		.text("To");
		
	// text label for the y axis
	svg.append("text")
		.attr("transform", "rotate(-90)")
		.attr("y", 0 - margin.left)
		.attr("x",0 - (height / 2))
		.attr("dy", "1em")
		.style("text-anchor", "middle")
		.text("From"); 
}

/*******************  FUNCTION  *********************/
function genPieDataFirstTouch(data)
{
	return [
		{
			"label": "Pinned",
			"value": data.metrics.firstTouch,
			"color": "rgb(44, 160, 44)",
		},{
			"label": "Unpinned",
			"value": data.metrics.unpinnedFirstTouch,
			"color": "rgb(255, 127, 14)",
		}
	];
}

/*******************  FUNCTION  *********************/
function genPieDataAccess(data)
{
	return [
		{
			"label": "Local",
			"value": data.metrics.localAccess,
			"color": "rgb(44, 160, 44)"
		},{
			"label": "Remote",
			"value": data.metrics.remoteAccess,
			"color": "red"
		},{
			"label": "Unpinned page",
			"value": data.metrics.unpinnedPageAccess,
			"color": "rgb(255, 187, 120)",
		},{
			"label": "Unpinned thread",
			"value": data.metrics.unpinnedThreadAccess,
			"color": "rgb(255, 127, 14)",
		},{
			"label": "Unpinned both",
			"value": data.metrics.unpinnedBothAccess,
			"color": "rgb(31, 119, 180)",
		},{
			"label": "Local MCDRAM",
			"value": data.metrics.localMcdramAccess,
			"color": "#FF79DE"
		},{
			"label": "Remote MCDRAM",
			"value": data.metrics.remoteMcdramAccess,
			"color": "#54017a"
		}
	];
}

/*******************  FUNCTION  *********************/
function loadInfos()
{
	$.getJSON( "/api/index/infos.json", function(data) {
		$("#summaryExe").text(data.exe);
		$("#summaryCommand").text(data.command);
		$("#summaryTool").text(data.tool);
		$("#summaryHostname").text(data.hostname);
		$("#summaryDate").text(data.date);
	})
	.fail(function(data) {
		numaprofHelper.logError("Fail to load infos");
	})
}

/*******************  FUNCTION  *********************/
function loadNumaTopo()
{
	$.getJSON( "/api/index/numa-topo.json", function(data) {
		for(var node in data)
		{
			var entry;
			if (data[node].isMcdram)
				entry = $("<tr><th>"+node+"</th><td>MCDRAM</td>");
			else
				entry = $("<tr><th>"+node+"</th><td>"+numaprofHelper.listToRange(data[node].cpus)+"</td>");
			$("#numaTopology").append(entry);
		}
	})
	.fail(function(data) {
		numaprofHelper.logError("Fail to load NUMA topology");
	})
}

/*******************  FUNCTION  *********************/
function loadProcessSummary()
{
	$.getJSON( "/api/index/process-summary.json", function(data) {
		setupPieChart("processFirstTouch",genPieDataFirstTouch(data));
		setupPieChart("processAccess",genPieDataAccess(data));
	})
	.fail(function(data) {
		numaprofHelper.logError("Fail to load process summary");
	})
}

/*******************  FUNCTION  *********************/
function loadAccessMatrix()
{
	$.getJSON( "/api/index/process-access-matrix.json", function(data) {
		setupHeadMap("accessMatrix",data);
	})
	.fail(function(data) {
		numaprofHelper.logError("Fail to load process summary");
	})
}

/*******************  FUNCTION  *********************/
function loadAllocationMatrix()
{
	$.getJSON( "/api/index/process-allocation-matrix.json", function(data) {
		setupHeadMap("allocationMatrix",data);
	})
	.fail(function(data) {
		numaprofHelper.logError("Fail to load process summary");
	})
}

/*******************  FUNCTION  *********************/
function gcMatrix()
{
  /*setupHeadMap("gcMatrix",
  [
    ["9648863", "3951517", "5072559", "2642785", "4239226", "12074132", "3794973", "3717223"],
    ["3237981", "3770201", "3836498", "3345781", "3506444", "17605091", "3445806", "3017789"],
    ["2916675", "2627400", "4088913", "3383237", "2709622", "5129044", "2693187", "2576240"],
    ["3570060", "2365710", "3106084", "5118023", "2754745", "13459310", "2751928", "2482771"],
    ["4192259", "3418220", "3208897", "2428751", "2828703", "12420607", "4244413", "3825811"],
    ["1573089", "1619333", "1796118", "2032166", "1363721", "4406077", "1487291", "1623910"],
    ["3206775", "3375574", "3995458", "2942271", "2577181", "10167878", "3410887", "3063807"],
    ["2188856", "1983897", "2342422", "2128538", "1566771", "4723187", "2040354", "2338496"]
  ]);*/
  /*setupHeadMap("gcMatrix",
    [["945856", "3711049", "623321", "633928", "625507", "540246", "2537339", "726322"],
    ["313089", "1488119", "315648", "271247", "364136", "272212", "1406835", "383303"],
    ["709310", "1532415", "595329", "550243", "710215", "586908", "4622900", "715433"],
    ["555727", "1882834", "554640", "517381", "589554", "443201", "3502659", "587814"],
    ["248193", "1141675", "269389", "249337", "231842", "241029", "859280", "495171"],
    ["3184809", "18561404", "3157320", "2769828", "3118587", "2879497", "10763978", "3381962"],
    ["197628", "271476", "187583", "216809", "224104", "111535", "1497468", "162358"],
    ["2366189", "2752041", "2352269", "2203029", "2649117", "2334829", "20419979", "2313621"]]
  );*/
  /*setupHeadMap("gcMatrix",
  [
   [3144260, 2450450, 1095933, 795605, 957579, 976588, 946527, 1337124], 
   [4904780, 3608267, 1784887, 1322561, 1535287, 1574385, 1591109, 2276512], 
   [3592384, 2404405, 1395004, 1029975, 1268470, 1205155, 1232649, 1762350], 
   [1423619, 933463, 577613, 429824, 526519, 507512, 507211, 713528], 
   [1436248, 1126032, 535798, 387144, 479127, 448946, 445965, 680306], 
   [2983620, 2251554, 1110201, 826969, 918905, 986032, 946071, 1366882], 
   [2871017, 2122048, 1111682, 819494, 1030178, 986343, 1029959, 1392131], 
   [7577674, 5313259, 2925960, 2192400, 2582962, 2572817, 2604941, 1]
  ]);*/
  setupHeadMap("gcMatrix", 
  [
    [115319629, 187822333, 96080112, 161919022, 58963214, 106049406, 73034248, 62212797],
    [106680234, 218633611, 103377237, 180527694, 64220834, 114647250, 77905555, 68961365],
    [106443718, 204341018, 123890369, 179304117, 65010254, 115451575, 79472384, 68829083],
    [98437761, 189674487, 95124650, 190478540, 59953355, 110860807, 73970855, 62246533], 
    [110991873, 213120753, 109634283, 190674458, 88200138, 119246423, 82727022, 71455021],
    [98563262, 192870729, 100500277, 174477747, 62849876, 126261314, 72743476, 65088340],
    [98819162, 194860168, 99606355, 160198298, 61715121, 112390608, 94505358, 65584188],
    [107158945, 209244225, 106725349, 180781934, 66375356, 119436216, 83587364, 88028245]
  ])
}

/*******************  FUNCTION  *********************/
function loadNumaPageStats()
{
	$.getJSON( "/api/index/numa-page-stats.json", function(data) {
		setupNumaPageStats(data);
	})
	.fail(function(data) {
		numaprofHelper.logError("Fail to load process summary");
	})
}

/*******************  FUNCTION  *********************/
function loadDistanceCnt()
{
	$.getJSON( "/api/index/process-distance-cnt.json", function(data) {
		setupDistanceCnt(data);
	})
	.fail(function(data) {
		numaprofHelper.logError("Fail to load process distance counters");
	})
}

/*******************  FUNCTION  *********************/
$(function() {
	loadInfos();
	loadNumaTopo();
	loadProcessSummary();
  //loadAccessMatrix();
  //loadAllocationMatrix();
  gcMatrix();
	loadNumaPageStats();
	loadDistanceCnt();
});
