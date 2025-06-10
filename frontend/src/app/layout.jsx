const Layout = ({ children }) => {
  return (
  	<html>
  		<head>
  		<title>Dynamic Ambulance</title>
        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" />
        <link href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.10.5/font/bootstrap-icons.css" rel="stylesheet" />
  		</head>
  	<body>
	    <div>
	      <main>{children}</main>
	    </div>
	</body>
    </html>
  )
}
export default Layout