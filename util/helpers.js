function isArray(x) {
    return x !== null && x !== undefined && x.constructor === Array;
}

function deconstructCellIndex(index, width) {
    return {
        x: index % width,
        y: Math.floor(index / width)
    };
}

function cellIndex(x, y, width) {
    return width * y + x;
}

function directionBetweenNodes(fromIndex, toIndex, width) {
	let fromCoord = deconstructCellIndex(fromIndex, width);
	let toCoord = deconstructCellIndex(toIndex, width);

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

module.exports = {
	isArray,
	deconstructCellIndex,
	cellIndex,
	directionBetweenNodes
};