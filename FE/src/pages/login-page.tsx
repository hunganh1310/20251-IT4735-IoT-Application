import { Box, Button, Checkbox, FormControlLabel, Paper, TextField, Typography } from "@mui/material"
import { useState } from "react"
import bg from "../assets/background.jpg";
import { useNavigate } from "react-router-dom";
import apiClient from "../api/axiosConfig";
import InputAdornment from '@mui/material/InputAdornment';
import IconButton from '@mui/material/IconButton';
import Visibility from '@mui/icons-material/Visibility';
import VisibilityOff from '@mui/icons-material/VisibilityOff';
import { toast } from "react-toastify";

const LoginPage = () => {
    const [email, setEmail] = useState("");
    const [password, setPassword] = useState("");
    const [remember, setRemember] = useState(false);
    const [showPassword, setShowPassword] = useState(false);
    const navigate = useNavigate();

    const handleLogin = async (e) => {
        e.preventDefault();
        try {
            const response = await apiClient.post('/auth/login', {
                email,
                password,
            })
            const responseData = response.data.data;
            console.log(responseData);

            const accessToken = responseData.access_token;
            const userObject = responseData.user;
            const userString = JSON.stringify(userObject);
            
            if (remember) {
                localStorage.setItem('accessToken', accessToken);
                localStorage.setItem('user', userString);
            } else {
                sessionStorage.setItem('accessToken', accessToken);
                sessionStorage.setItem('user', userString);
            }
            console.log(accessToken);
            toast.success("Đăng ký thành công!");
            navigate('/dashboard');
        } catch (err) {
            toast.error("Đăng nhập thất bại!");
            console.log("Error", err);
        }
    }

    return (
        <Box
            sx={{
                height: "100vh",
                display: "flex",
                alignItems: "center",
                justifyContent: "center",
                backgroundImage: `url(${bg})`,
                backgroundSize: "cover",
                backgroundPosition: "center",
            }}
        >
            <Paper
                elevation={10}   //tao hieu ung do bong
                sx={{
                    p: 5,
                    width: 550,
                    borderRadius: 4,
                    backgroundColor: "rgba(255, 255, 255, 0.98)",
                }}
            >
                <Typography variant="h4" align="center" mb={3} fontWeight="bold">
                    Đăng nhập
                </Typography>

                <form onSubmit={handleLogin}>
                    <TextField
                        label="Email"
                        type="email"
                        value={email}
                        onChange={(e) => setEmail(e.target.value)}
                        fullWidth
                        required
                        margin="normal"
                        sx = {{
                            "& .MuiOutlinedInput-root": {
                                borderRadius: 2
                            }
                        }}
                    />
                    <TextField
                        label="Mật khẩu"
                        type={ showPassword ? "text" : "password" }
                        value={password}
                        onChange={(e) => setPassword(e.target.value)}
                        fullWidth
                        required
                        margin="normal"
                        sx = {{
                            "& .MuiOutlinedInput-root": {
                                borderRadius: 2
                            }
                        }}
                        InputProps={{
                            endAdornment: (
                                <InputAdornment position="end">
                                    <IconButton
                                        onClick={() => setShowPassword(!showPassword)}
                                        edge="end"
                                    >
                                        {showPassword ? <Visibility /> : <VisibilityOff />}
                                    </IconButton>
                                </InputAdornment>
                            ),
                        }}
                    />

                    <Box sx={{display: 'flex', flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center',}}>
                        <FormControlLabel 
                            control={
                                <Checkbox
                                    checked={remember}
                                    onChange={(e) => setRemember(e.target.checked)}
                                />
                            } 
                            label={
                                <Typography sx={{ fontSize: "0.875rem" }}>
                                    Ghi nhớ mật khẩu
                                </Typography>
                            }
                        />
                        <Typography 
                            variant="body2"
                            sx={{
                                "&:hover": {
                                    textDecoration: 'underline',
                                    cursor: 'pointer',
                                }
                            }}
                        >
                            <a href="/forgot-password" style={{textDecoration: 'underline', cursor: 'pointer',color: "#1976d2"}}>Quên mật khẩu?</a>
                        </Typography>
                    </Box>

                    <Button
                        type="submit"
                        variant="contained"
                        color="primary"
                        fullWidth
                        sx={{ 
                            mt: 2, 
                            py: 1.2,    
                            borderRadius: 2,
                        }}
                    >
                        Đăng nhập
                    </Button>
                </form>


                <Typography
                    variant="body2"
                    align="center"
                    sx={{ mt: 3, color: "text.secondary" }}
                >
                    Chưa có tài khoản?{" "}
                    <a href="/register" style={{ textDecoration: "none", color: "#1976d2" }}>
                        Đăng ký ngay
                    </a>
                </Typography>
            </Paper>
        </Box>
    )
}

export default LoginPage