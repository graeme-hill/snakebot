let helpers = require("./helpers");

function getIndex(coord, gameState) {
	return helpers.cellIndex(coord.x, coord.y, gameState.width);
}

// Just calculate the distance if there were no barriers.
function heuristicCostEstimate(start, goal) {
	return Math.abs(start.x - goal.x) + Math.abs(start.y - goal.y);
}

function lowestFScoreInSet(set, fScore) {
	let lowest = null;
	let lowestIndex = null;
	set.forEach((index) => {
		let score = fScore[index];
		if (score !== undefined && score !== null) {
			if (lowest === null || score < lowest) {
				lowest = score;
				lowestIndex = index;
			}
		}
	});
	return lowestIndex;
}

function directionBetweenNodes(fromIndex, toIndex, width) {
	let fromCoord = helpers.deconstructCellIndex(fromIndex, width);
	let toCoord = helpers.deconstructCellIndex(toIndex, width);

	// Find first sensible direction assuming that they are adjacent and not diagonal.
	if (fromCoord.x < toCoord.x) {
		return "right";
	}
	if (toCoord.x < fromCoord.x) {
		return "left";
	}
	if (fromCoord.y < toCoord.y) {
		return "down";
	}
	return "up";
}

function reconstructPath(cameFrom, currentIndex, width) {
	let moves = [];
	while (currentIndex in cameFrom) {
		let nextIndex = cameFrom[currentIndex];
		let direction = directionBetweenNodes(nextIndex, currentIndex, width);
		moves.unshift(direction);
		currentIndex = nextIndex;
	}
	return moves;
}

function getNeighbors(index, width, height) {
	let coord = helpers.deconstructCellIndex(index, width);
	let result = [];

	// left - only if not in first column
	if (coord.x > 0) {
		result.push(index - 1);
	}

	// right - only if not in last column
	if (coord.x < width - 1) {
		result.push(index + 1);
	}

	// up - only if not on first row
	if (coord.y > 0) {
		result.push(index - width);
	}

	// down - only if not on last row
	if (coord.y < height - 1) {
		result.push(index + width);
	}

	return result;
}

// Partially stolen from the pseudo-code in the wikipedia page for A*:
// https://en.wikipedia.org/wiki/A*_search_algorithm#Pseudocode
function shortestPath(start, goal, gameState) {
	let startIndex = getIndex(start, gameState);
	let goalIndex = getIndex(goal, gameState);

	// Nodes/cells that have already been evaluated.
	let closedSet = new Set();

	// Nodes/cells that are not yet evaluated but have been discovered. Only the start
	// node is discovered so far.
	let openSet = new Set([ startIndex ]);

	// For each node store the node it can most efficiently be reached from.
	let cameFrom = {};

	// For each node the cost of getting from the start to that node.
	let gScore = {};

	// Already at start so cost of going there is zero.
	gScore[startIndex] = 0;

	// For each node the cost of getting from start to goal passing through that node.
	// Depends on heuristics. Not guaranteed to be precise.
	let fScore = {};

	// fScore for start node is 100% heuristic.
	fScore[startIndex] = heuristicCostEstimate(start, goal);

	while (openSet.size > 0) {
		console.log("A* loop. open: " + openSet.size);
		let currentIndex = lowestFScoreInSet(openSet, fScore);

		if (currentIndex === goalIndex) {
			return reconstructPath(cameFrom, currentIndex, gameState.width);
		}

		openSet.delete(currentIndex);
		closedSet.add(currentIndex);

		let neighbors = getNeighbors(currentIndex, gameState.width, gameState.height);
		for (let neighborIndex of neighbors) {

			// Don't revisit the same node we've already been to.
			if (closedSet.has(neighborIndex)) {
				continue;
			}

			// Check if we have discovered a new node.
			if (!openSet.has(neighborIndex)) {
				openSet.add(neighborIndex);
			}

			// Distance between neighbors is always 1, hence the +1.
			let tentativeGScore = gScore[currentIndex] + 1;

			// If there's already a better gScore for this node then we're not on the
			// ideal path so bail.
			if (tentativeGScore >= gScore[neighborIndex]) {
				continue;
			}

			// For now this is the best path to this node so record it.
			cameFrom[neighborIndex] = currentIndex;
			gScore[neighborIndex] = tentativeGScore;
			fScore[neighborIndex] = gScore[neighborIndex] + heuristicCostEstimate(
				snake.deconstructCellIndex(neighborIndex, gameState.width), goal);
		}
	}

	// Finished searching and could not find a path :(
	return null;
}

module.exports = {
	shortestPath
};