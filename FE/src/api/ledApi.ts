import apiClient from "./axiosConfig";

export const ledApi = {
    updateLed: (id: number, data: any) => apiClient.put(`/leds/${id}`, data),
};
