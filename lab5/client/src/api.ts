export async function fetchCurrentTemperature(): Promise<number> {
	const response = await fetch("/temperature");
	const data = await response.json();
	return data.temperature;
}

export async function fetchStatistics(
	period: "hourly" | "daily"
): Promise<any[]> {
	const response = await fetch(`/statistics?period=${period}`);
	const data = await response.json();
	return data;
}
