'use client';

import { useEffect, useState } from 'react';
import Link from 'next/link';
import { Modal, Button } from 'react-bootstrap';
import { useRouter } from 'next/navigation';

export default function PatientsPage() {
  const [patients, setPatients] = useState([]);
  const [search, setSearch] = useState('');
  const [filter, setFilter] = useState('');
  const [selected, setSelected] = useState(null);
  const [loading, setLoading] = useState(true);
  const router = useRouter();

  // Fetch patients
  const fetchPatients = async () => {
    setLoading(true);
    try {
      const res = await fetch('/api/mongo?collection=patients');
      const data = await res.json();
      setPatients(data.data || []);
    } catch (err) {
      console.error('Failed to fetch patients:', err);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchPatients();
  }, []);

  const handleDelete = async (patient) => {
    if (!patient._id) {
      alert('Missing patient ID');
      return;
    }

    const confirmDelete = window.confirm(`Delete patient "${patient.name}"?`);
    if (!confirmDelete) return;

    try {
      const res = await fetch('/api/mongo?collection=patients', {
        method: 'DELETE',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          filter: { _id: typeof patient._id === 'object' ? patient._id.$oid || patient._id : patient._id },
        }),
      });

      const result = await res.json();

      if (res.ok) {
        alert('✅ Patient deleted.');
        fetchPatients();
      } else {
        alert('❌ Delete failed: ' + result?.error);
      }
    } catch (err) {
      alert('❌ Network error');
      console.error(err);
    }
  };

  const handleEdit = (patient) => {
    const params = new URLSearchParams();
    Object.entries(patient).forEach(([key, value]) => {
      if (typeof value === 'string' || typeof value === 'number') {
        params.append(key, value);
      }
    });
    router.push(`/AdminDashboard/add_patient?${params.toString()}`);
  };

  const filteredPatients = patients.filter((p) => {
    const matchSearch =
      p.name?.toLowerCase().includes(search.toLowerCase()) ||
      p.patientId?.toString().includes(search);
    const matchGender = filter ? p.gender?.toLowerCase() === filter.toLowerCase() : true;
    return matchSearch && matchGender;
  });

  return (
    <div className="container-fluid py-4">
      <div className="d-flex justify-content-between align-items-center mb-4">
        <h2 className="fw-bold text-primary mb-0">Patients</h2>
        <nav aria-label="breadcrumb">
          <ol className="breadcrumb mb-0">
            <li className="breadcrumb-item"><a href="/AdminDashboard">Dashboard</a></li>
            <li className="breadcrumb-item active">Patients</li>
          </ol>
        </nav>
      </div>

      <div className="card mb-4">
        <div className="card-body row g-3">
          <div className="col-md-6">
            <input
              type="text"
              className="form-control"
              placeholder="Search by name or ID"
              value={search}
              onChange={(e) => setSearch(e.target.value)}
            />
          </div>
          <div className="col-md-4">
            <select
              className="form-control"
              value={filter}
              onChange={(e) => setFilter(e.target.value)}
            >
              <option value="">All Genders</option>
              <option value="Male">Male</option>
              <option value="Female">Female</option>
              <option value="Other">Other</option>
            </select>
          </div>
          <div className="col-md-2">
            <Link href="/AdminDashboard/add_patient" className="btn btn-primary w-100">
              <i className="bi bi-person-plus me-2" /> Add Patient
            </Link>
          </div>
        </div>
      </div>

      <div className="card">
        <div className="card-body table-responsive">
          {loading ? (
            <p className="text-center">Loading...</p>
          ) : filteredPatients.length === 0 ? (
            <p className="text-center text-muted">No patients found.</p>
          ) : (
            <table className="table table-striped align-middle">
              <thead>
                <tr>
                  <th>Name</th>
                  <th>ID</th>
                  <th>Age</th>
                  <th>Gender</th>
                  <th>Phone</th>
                  <th>Hospital</th>
                  <th className="text-end">Actions</th>
                </tr>
              </thead>
              <tbody>
                {filteredPatients.map((p, i) => (
                  <tr key={i}>
                    <td>{p.name}</td>
                    <td>{p.patientId}</td>
                    <td>{p.age}</td>
                    <td>{p.gender || 'N/A'}</td>
                    <td>{p.phoneNumber || 'N/A'}</td>
                    <td>{p.hospitalAssigned || 'N/A'}</td>
                    <td className="text-end">
                      <button
                        className="btn btn-sm btn-outline-success me-2"
                        onClick={() => handleEdit(p)}
                      >
                        <i className="bi bi-pencil-square me-1"></i>
                      </button>
                      <button
                        className="btn btn-sm btn-outline-danger me-2"
                        onClick={() => handleDelete(p)}
                      >
                        <i className="bi bi-trash me-1"></i>
                      </button>
                      <button
                        className="btn btn-sm btn-outline-info"
                        onClick={() => setSelected(p)}
                      >
                        <i className="bi bi-eye me-1"></i> View
                      </button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
      </div>

      {/* View More Modal */}
      {selected && (
        <Modal show onHide={() => setSelected(null)} centered>
          <Modal.Header closeButton>
            <Modal.Title>Patient Details</Modal.Title>
          </Modal.Header>
          <Modal.Body>
            {Object.entries(selected).map(([key, val]) => (
              <p key={key}>
                <strong>{key}:</strong> {val?.toString() || 'N/A'}
              </p>
            ))}
          </Modal.Body>
          <Modal.Footer>
            <Button variant="secondary" onClick={() => setSelected(null)}>
              Close
            </Button>
          </Modal.Footer>
        </Modal>
      )}
    </div>
  );
}
