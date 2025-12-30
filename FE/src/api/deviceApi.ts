import apiClient from "./axiosConfig";

export const deviceApi = {
    getAllDevices: () => apiClient.get('/devices'),
    createDevice: (data: any) => apiClient.post('/devices', data),
    updateDevice: (id: number, data: any) => apiClient.put(`/devices/${id}`, data),
    deleteDevice: (id: number) => apiClient.delete(`/devices/${id}`),
};
