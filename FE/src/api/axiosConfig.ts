import axios from "axios";;

const apiClient = axios.create({
  baseURL: 'http://localhost:3000',
});

apiClient.interceptors.request.use(
  (config) => {
    let accessToken = localStorage.getItem('accessToken');

    if(!accessToken){
      accessToken = sessionStorage.getItem('accessToken');
    }

    if(accessToken){
      config.headers['Authorization'] = `Bearer ${accessToken}`;
    }
    
    return config;
  },
  (error) => {
    return Promise.reject(error);
  }
);

export default apiClient;