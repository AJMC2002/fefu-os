import { fetchCurrentTemperature, fetchStatistics } from "./api";
import { renderChart, updateTable } from "./chart";

async function main() {
	// Fetch and display current temperature
	const currentTemp = await fetchCurrentTemperature();
	document.getElementById("current-temp")!.textContent = `${currentTemp} °C`;

	// Fetch and display statistics
	const stats = await fetchStatistics("hourly");
	renderChart(stats);
	updateTable(stats);
}

main();
